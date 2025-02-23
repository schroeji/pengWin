#include "hotkey.hpp"
#include "manager.hpp"

#include <iostream>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <boost/thread.hpp>
#include <functional>
#include <linux/input.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <pthread.h>

using namespace std;

int x_error_handler(Display* d, XErrorEvent* e) {
  // throw runtime_error("X Error");
  return 0;
}

X11HotkeyManager::X11HotkeyManager(GameManager& csgo) : csgo(csgo),
                                                  settings(Settings::getInstance()) {
  std::cout << "Starting HotkeyManager" << std::endl;
  // enable multi threading for X11
  XInitThreads();
  // sets the error handler so the program does not crash for invalid grabs
  XSetErrorHandler(x_error_handler);
  display = XOpenDisplay(NULL);
  rootWindow = DefaultRootWindow(display);
  csWindow = findCSWindow();
}

X11HotkeyManager::~X11HotkeyManager() {}

void X11HotkeyManager::bind(string key, boost::function<void(unsigned int)> func){
  bool isKeyboard = (key.compare(0, 5, "mouse") &&
                     key.compare(0, 5, "MOUSE") &&
                     key.compare(0, 5, "Mouse"));
  if (isKeyboard) {
    KeySym keysym = XStringToKeysym(key.c_str());
    unsigned int keycode = XKeysymToKeycode(display, keysym);
    bindings.insert(std::pair<unsigned int, boost::function<void(unsigned int)>>(keycode, func));
    holding_key.insert(std::pair<unsigned int, bool>(keycode, false));
    if (settings.debug) cout << "Bound key: " << XKeysymToString(keysym) << endl;
  } else { // mouse key
    // use mouseListOffset as offset for the mouse button keycodes
    // this is kinda hackish, but this way we dont need another map
    // change 1 to mosue button
    int button_nr = stoi(key.substr(5, key.length()));
    unsigned int keycode = mouseListOffset + button_nr;
    bindings.insert(std::pair<unsigned int, boost::function<void(unsigned int)>>(keycode, func));
    holding_key.insert(std::pair<unsigned int, bool>(keycode, false));
    if (settings.debug) cout << "Bound mouse key: " << button_nr << endl;
  }
  listening = false;
}

void X11HotkeyManager::unbind(string key) {
  bool isKeyboard = (key.compare(0, 5, "mouse") &&
                     key.compare(0, 5, "MOUSE") &&
                     key.compare(0, 5, "Mouse"));
  unsigned int keycode;
  if (isKeyboard) {
    KeySym keysym = XStringToKeysym(key.c_str());
    keycode = XKeysymToKeycode(display, keysym);
  } else {
    int button_nr = stoi(key.substr(5, key.length()));
    keycode = mouseListOffset + button_nr;
  }
  bindings.erase(keycode);
}

void X11HotkeyManager::startListen() {
  if (bindings.size() < 1)
    return;
  listening = true;
  keyPressListener = boost::thread(boost::bind(&X11HotkeyManager::keyPressListen, this));
  mousePressListener = boost::thread(boost::bind(&X11HotkeyManager::mousePressListen, this));
  this_thread::sleep_for(chrono::milliseconds(3));
  if (settings.debug) cout << "Started Hotkey listener..." << endl;
}

void X11HotkeyManager::stopListen() {
  if (!listening)
    return;
  listening = false;
  // keyPressListener.join();
  pthread_cancel(keyPressListener.native_handle());
  if (settings.debug) cout << "Killed keyPressListener..." << endl;
  // mousePressListener.join();
  pthread_cancel(mousePressListener.native_handle());
  if (settings.debug) cout << "Killed mousePressListener..." << endl;
  for (map<unsigned int, bool>::iterator it = holding_key.begin(); it != holding_key.end(); it++) {
    it->second = false;
  }
  if (settings.debug) cout << "Stopped Hotkey listener." << endl;
}

unsigned int X11HotkeyManager::eventCodeToMouseButton(unsigned int code) {
  // -271 for event.code and +mouseListOffset to map to the right position in bindings
  return code + mouseListOffset - 271;
}

void X11HotkeyManager::mousePressListen() {
  int fd;
  struct input_event event;

  if((fd = open(settings.mouse_file.c_str(), O_RDONLY)) == -1) {
    perror("Error while opening device");
    exit(EXIT_FAILURE);
  }
  while (listening && csgo.isOnServer()) {
    // event value == 1 => button press
    // event value == 0 => button release
    read(fd, &event, sizeof(struct input_event));
    unsigned int keycode = eventCodeToMouseButton(event.code);
    // if (settings.debug) cout << "Mouse event:" << keycode << endl;
    // skip if no mapping present
    if (bindings.find(keycode) == bindings.end()) {
      continue;
    }
    Window activeWin = activeWindow();
    if (!holding_key.at(keycode) && event.value > 0 && activeWin == csWindow) { // button press
      boost::function<void(unsigned int)> func;
      try {
        func = bindings.at(keycode);
      } catch (const out_of_range& e) {
        throw runtime_error("No binding for pressed key. This should not happen.");
      }
      holding_key[keycode] = true;
      threads[keycode] = boost::thread(boost::bind(&X11HotkeyManager::callLoop,
                                                   this,
                                                   keycode,
                                                   func));
    } else if (holding_key.at(keycode) && event.value == 0) { // button release
      holding_key[keycode] = false;
      // when debugging wait for thread to terminate
      // if (settings.debug) threads[keycode].join();
    } else if (activeWin != csWindow) {
      if (settings.debug) cout << "Ignoring because other window focused." << endl;
    } else {
      if (settings.debug) cout << "WARNING: inconsistent mouse state" << endl;
      holding_key.at(keycode) = false;
    }
  }
}

void X11HotkeyManager::keyPressListen() {
  unsigned int modifiers;
  int pointer_mode = GrabModeAsync;
  int keyboard_mode = GrabModeAsync;
  bool owner_events = True;
  XEvent event;

  for (map<unsigned int, boost::function<void(unsigned int)>>::iterator it = bindings.begin(); it != bindings.end(); it++) {

    modifiers = AnyModifier;
    if (settings.debug) cout << "Grabbing keycode:" << it->first << endl;
    XUngrabKey(display, it->first, modifiers, csWindow);
    // grab with anymodifer (fails for space)
    XGrabKey(display, it->first, modifiers, csWindow, owner_events, pointer_mode,
             keyboard_mode);
    // grab with caps_lock
    // modifiers = LockMask;
    // XGrabKey(display, it->first, modifiers, csWindow, owner_events, pointer_mode,
             // keyboard_mode);
    // grab without modifier
    // modifiers = 0;
    // XGrabKey(display, it->first, modifiers, csWindow, owner_events, pointer_mode,
             // keyboard_mode);
  }
  while (listening && csgo.isOnServer()) {
    XNextEvent(display, &event);
    // without this events are consumed; idk why even with owner_events = true
    forwardEvent(event);
    if (event.type == KeyPress) {
      if (settings.debug) cout << "HotKey pressed:" << event.xbutton.button << endl;
      if (holding_key.find(event.xbutton.button) == holding_key.end() ||
          holding_key.at(event.xbutton.button)) {
        continue;
      }
      boost::function<void(unsigned int)> func;
      try {
        func = bindings.at(event.xbutton.button);
      } catch (const out_of_range& e) {
        throw runtime_error("No binding for pressed Key. This should not happen.");
      }
      holding_key[event.xbutton.button] = true;
      threads[event.xbutton.button] = boost::thread(boost::bind(&X11HotkeyManager::callLoop,
                                                                this,
                                                                event.xbutton.button,
                                                                func));
    } else if (event.type == KeyRelease &&
               holding_key.find(event.xbutton.button) != holding_key.end() &&
               holding_key.at(event.xbutton.button)) {
      holding_key[event.xbutton.button] = false;
      // when debugging wait for thread to terminate
      // if (settings.debug) threads[event.xbutton.button].join();
      if (settings.debug) cout << "joined thread" << endl;
    }
  }
}

void X11HotkeyManager::callLoop(unsigned int keycode, boost::function<void(unsigned int)> func) {
  unsigned int call_count = 0;
  while (holding_key.at(keycode)) {
    func(call_count);
    call_count++;
  }
}

void X11HotkeyManager::forwardEvent(XEvent event) {
  XSendEvent(display, csWindow, true, NoEventMask, &event);
  XFlush(display);
}

Window X11HotkeyManager::findCSWindow() {
  FILE* in;
  char buf[128];
  string cmd = "xdotool search --any --pid \"" + to_string(csgo.getMemoryAccess().getPid()) + "\"";
  in = popen(cmd.c_str(), "r");
  fgets(buf, 128, in);
  pclose(in);
  Window w = (Window) strtoul(buf, NULL, 10);
  if (settings.debug) cout << "Found CSGO Window ID: "<< dec << w << endl;
  if (w == 0)
    throw runtime_error("Could not find window or xdotool is not installed.");
  return w;
}

Window X11HotkeyManager::activeWindow() {
  FILE* in;
  char buf[128];
  string cmd = "xdotool getWindowfocus";
  in = popen(cmd.c_str(), "r");
  fgets(buf, 128, in);
  pclose(in);
  Window w = (Window) strtoul(buf, NULL, 10);
  if (w == 0)
    throw runtime_error("Could not find window or xdotool is not installed.");
  return w;
}

bool X11HotkeyManager::isListening() {
  return listening;
}
