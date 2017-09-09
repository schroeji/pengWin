#include "hotkey.hpp"
#include "manager.hpp"

#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>
#include <boost/thread.hpp>
#include <functional>

using namespace std;

int x_error_handler(Display* d, XErrorEvent* e) {
  // throw runtime_error("X Error");
  return 0;
}

HotkeyManager::HotkeyManager(GameManager csgo) : csgo(csgo),
                                                 settings(Settings::getInstance()) {
  XSetErrorHandler(x_error_handler); //
  display = XOpenDisplay(NULL);
  rootWindow = DefaultRootWindow(display);
}

HotkeyManager::~HotkeyManager() {}

void HotkeyManager::bind(KeySym key, boost::function<void()> func){
  unsigned int keycode = XKeysymToKeycode(display, key);
  bindings.insert(std::pair<unsigned int, boost::function<void()>>(keycode, func));
  holding_key.insert(std::pair<unsigned int, bool>(keycode, false));
  if (settings.debug) cout << "Bound key: " << XKeysymToString(key) << endl;
}

void HotkeyManager::startListen() {
  if (bindings.size() < 1)
    return;
  keyPressListener = boost::thread(boost::bind(&HotkeyManager::keyPressListen, this));
  if (settings.debug) cout << "Started Hotkey listener..." << endl;
}

void HotkeyManager::keyPressListen() {
  unsigned int modifiers;
  int pointer_mode = GrabModeAsync;
  int keyboard_mode = GrabModeAsync;
  bool owner_events = false;
  XEvent event;

  for (map<unsigned int, boost::function<void()>>::iterator it = bindings.begin(); it != bindings.end(); it++) {
    modifiers = AnyModifier;
    XUngrabKey(display, it->first, modifiers, rootWindow);
    // grab with anymodifer (fails for space)
    XGrabKey(display, it->first, modifiers, rootWindow, owner_events, pointer_mode,
             keyboard_mode);
    // grab with caps_lock
    modifiers = LockMask;
    XGrabKey(display, it->first, modifiers, rootWindow, owner_events, pointer_mode,
             keyboard_mode);
    // grab without modifier
    modifiers = 0;
    XGrabKey(display, it->first, modifiers, rootWindow, owner_events, pointer_mode,
             keyboard_mode);
  }
  XSelectInput(display, rootWindow, KeyPressMask);
  while (csgo.isOnServer()) {
    XNextEvent(display, &event);
    if (event.type == KeyPress) {
      if (settings.debug) cout << "key pressed..." << endl;
      try {
        if (holding_key.at(event.xbutton.button)) {
          continue;
        }
      } catch (out_of_range e) {
        forwardEvent(event);
        if (settings.debug) cout << "Event forwarded" << endl;
        continue;
      }
      boost::function<void()> func;
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
      try {
        if (holding_key.at(event.xbutton.button)) {
          holding_key[event.xbutton.button] = false;
          // maybe remove
          threads[event.xbutton.button].join();
        }
      } catch (out_of_range e) {
        forwardEvent(event);
      }
    }
  }
}

void HotkeyManager::callLoop(unsigned int keycode, boost::function<void()> func) {
  while (holding_key.at(keycode)) {
    func();
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
