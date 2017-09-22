#include "hotkey.hpp"
#include "manager.hpp"

#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <boost/thread.hpp>
#include <functional>
#include <linux/input.h>
#include <fcntl.h>
#include <iostream>


using namespace std;

int x_error_handler(Display* d, XErrorEvent* e) {
  // throw runtime_error("X Error");
  return 0;
}

HotkeyManager::HotkeyManager(GameManager& csgo) : csgo(csgo),
                                                  settings(Settings::getInstance()) {
  // enable multi threading for X11
  XInitThreads();
  // sets the error handler so the program does not crash for invalid grabs
  XSetErrorHandler(x_error_handler);
  display = XOpenDisplay(NULL);
  rootWindow = DefaultRootWindow(display);
  csWindow = findCSWindow();
}

HotkeyManager::~HotkeyManager() {}

void HotkeyManager::bind(string key, boost::function<void(unsigned int)> func){
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
    int button_nr = stoi(key.substr(5, 1));
    unsigned int keycode = mouseListOffset + button_nr;
    bindings.insert(std::pair<unsigned int, boost::function<void(unsigned int)>>(keycode, func));
    holding_key.insert(std::pair<unsigned int, bool>(keycode, false));
  }

}

void HotkeyManager::startListen() {
  if (bindings.size() < 1)
    return;
  keyPressListener = boost::thread(boost::bind(&HotkeyManager::keyPressListen, this));
  mousePressListener = boost::thread(boost::bind(&HotkeyManager::mousePressListen, this));
  if (settings.debug) cout << "Started Hotkey listener..." << endl;
}

unsigned int HotkeyManager::eventCodeToMouseButton(unsigned int code) {
  // -271 for event.code and +mouseListOffset to map to the irght position in bindings
  return code + mouseListOffset - 271;
}

void HotkeyManager::mousePressListen() {
  int fd;
  struct input_event event;

  if((fd = open(settings.mouse_file.c_str(), O_RDONLY)) == -1) {
    perror("opening device");
    exit(EXIT_FAILURE);
  }

  while (csgo.isOnServer()) {
    // event value == 1 => button press
    // event value == 0 => button release
    read(fd, &event, sizeof(struct input_event));
    unsigned int keycode = eventCodeToMouseButton(event.code);
    // skip if no mapping present
    if (bindings.find(keycode) == bindings.end())
      continue;

    if (!holding_key.at(keycode) && event.value == 1) { // button press
      boost::function<void(unsigned int)> func;
      try {
        func = bindings.at(keycode);
      } catch (out_of_range e) {
        throw runtime_error("No binding for pressed key.");
      }
      holding_key[keycode] = true;
      threads[keycode] = boost::thread(boost::bind(&HotkeyManager::callLoop,
                                                   this,
                                                   keycode,
                                                   func));
    } else if (holding_key.at(keycode) && event.value == 0) { // button release
      holding_key[keycode] = false;
      // when debugging wait for thread to terminate
      if (settings.debug) threads[keycode].join();
    } else {
      if (settings.debug) cout << "WARNING: inconsistent mouse state" << endl;
      holding_key.at(keycode) = false;
    }
  }
}

void HotkeyManager::keyPressListen() {
  unsigned int modifiers;
  int pointer_mode = GrabModeAsync;
  int keyboard_mode = GrabModeAsync;
  bool owner_events = false;
  XEvent event;

  for (map<unsigned int, boost::function<void(unsigned int)>>::iterator it = bindings.begin(); it != bindings.end(); it++) {
    modifiers = AnyModifier;
    XUngrabKey(display, it->first, modifiers, csWindow);
    // grab with anymodifer (fails for space)
    XGrabKey(display, it->first, modifiers, csWindow, owner_events, pointer_mode,
             keyboard_mode);
    // grab with caps_lock
    modifiers = LockMask;
    XGrabKey(display, it->first, modifiers, csWindow, owner_events, pointer_mode,
             keyboard_mode);
    // grab without modifier
    modifiers = 0;
    XGrabKey(display, it->first, modifiers, csWindow, owner_events, pointer_mode,
             keyboard_mode);
  }
  XSelectInput(display, csWindow, KeyPressMask);
  while (csgo.isOnServer()) {
    XNextEvent(display, &event);
    if (event.type == KeyPress) {
      if (settings.debug) cout << "key pressed..." << endl;
      if (holding_key.find(event.xbutton.button) != holding_key.end()) {
        if (holding_key.at(event.xbutton.button))
          continue;
      } else {
        forwardEvent(event);
        if (settings.debug) cout << "Event forwarded" << endl;
        continue;
      }
      boost::function<void(unsigned int)> func;
      try {
        func = bindings.at(event.xbutton.button);
      } catch (out_of_range e) {
        throw runtime_error("No binding for pressed Key");
      }
      holding_key[event.xbutton.button] = true;
      threads[event.xbutton.button] = boost::thread(boost::bind(&HotkeyManager::callLoop,
                                                                this,
                                                                event.xbutton.button,
                                                                func));
    } else if(event.type == KeyRelease) {
      if (holding_key.find(event.xbutton.button) != holding_key.end()) {
        if (holding_key.at(event.xbutton.button)) {
          holding_key[event.xbutton.button] = false;
          // when debugging wait for thread to terminate
          if (settings.debug) threads[event.xbutton.button].join();
        }
      } else {
        forwardEvent(event);
      }
    }
  }
}

void HotkeyManager::callLoop(unsigned int keycode, boost::function<void(unsigned int)> func) {
  unsigned int call_count = 0;
  while (holding_key.at(keycode)) {
    func(call_count);
    call_count++;
  }
}

void HotkeyManager::forwardEvent(XEvent event) {
  while (event.xbutton.subwindow) {
    event.xbutton.window = event.xbutton.subwindow;
    XQueryPointer (display, event.xbutton.window,
                   &event.xbutton.root, &event.xbutton.subwindow,
                   &event.xbutton.x_root, &event.xbutton.y_root,
                   &event.xbutton.x, &event.xbutton.y,
                   &event.xbutton.state);
  }
  XSendEvent(display, PointerWindow, true, NoEventMask, &event);
  XFlush(display);
}

Window HotkeyManager::findCSWindow() {
  FILE* in;
  char buf[128];
  string cmd = "xdotool search --any --pid \"" + to_string(csgo.getMemoryAccess().getPid()) + "\"";
  in = popen(cmd.c_str(), "r");
  fgets(buf, 128, in);
  pclose(in);
  Window w = (Window) strtoul(buf, NULL, 10);
  if (w == 0)
    throw runtime_error("Could not find window or xdotool is not installed.");
  return w;
}
