#include "wayland_hotkey.hpp"
#include "manager.hpp"

#include <iostream>
#include <linux/input.h>
#include <boost/thread.hpp>
#include <fcntl.h>
#include <functional>
#include <chrono>
#include <thread>
#include <pthread.h>

using namespace std;

WaylandHotkeyManager::~WaylandHotkeyManager() {
    if (keyboard) {
            wl_keyboard_release(keyboard);
        }
    // if (xkb_keymap) {
    //     xkb_keymap_unref(xkb_keymap);
    // }
    // if (xkb_context) {
    //     xkb_context_unref(xkb_context);
    // }
    if (display) {
        wl_display_disconnect(display);
    }
}

void WaylandHotkeyManager::bind(string key, boost::function<void(unsigned int)> func){

  bool isKeyboard = (key.compare(0, 5, "mouse") &&
                     key.compare(0, 5, "MOUSE") &&
                     key.compare(0, 5, "Mouse"));
  // if (isKeyboard) {
  //   KeySym keysym = XStringToKeysym(key.c_str());
  //   unsigned int keycode = XKeysymToKeycode(display, keysym);
  //   bindings.insert(std::pair<unsigned int, boost::function<void(unsigned int)>>(keycode, func));
  //   holding_key.insert(std::pair<unsigned int, bool>(keycode, false));
  //   if (settings.debug) cout << "Bound key: " << XKeysymToString(keysym) << endl;
  // } else { // mouse key
  //   // use mouseListOffset as offset for the mouse button keycodes
  //   // this is kinda hackish, but this way we dont need another map
  //   // change 1 to mosue button
  //   int button_nr = stoi(key.substr(5, key.length()));
  //   unsigned int keycode = mouseListOffset + button_nr;
  //   bindings.insert(std::pair<unsigned int, boost::function<void(unsigned int)>>(keycode, func));
  //   holding_key.insert(std::pair<unsigned int, bool>(keycode, false));
  //   if (settings.debug) cout << "Bound mouse key: " << button_nr << endl;
  // }
  // listening = false;
}

void WaylandHotkeyManager::unbind(string key) {
  // bool isKeyboard = (key.compare(0, 5, "mouse") &&
  //                    key.compare(0, 5, "MOUSE") &&
  //                    key.compare(0, 5, "Mouse"));
  // unsigned int keycode;
  // if (isKeyboard) {
  //   KeySym keysym = XStringToKeysym(key.c_str());
  //   keycode = XKeysymToKeycode(display, keysym);
  // } else {
  //   int button_nr = stoi(key.substr(5, key.length()));
  //   keycode = mouseListOffset + button_nr;
  // }
  // bindings.erase(keycode);
}

void WaylandHotkeyManager::startListen() {
//   display = wl_display_connect(nullptr);
//   if (!display) {
//       std::cerr << "Unable to connect to Wayland display." << std::endl;
//   }

//   registry = wl_display_get_registry(display);
//   wl_registry_add_listener(registry, &registry_listener, this);
//   wl_display_roundtrip(display);

//   if (!seat) {
//       std::cerr << "Unable to find Wayland seat." << std::endl;
//   }

//   listening = true;
//   keyPressListener = boost::thread(boost::bind(&WaylandHotkeyManager::keyPressListen, this));
//   mousePressListener = boost::thread(boost::bind(&WaylandHotkeyManager::mousePressListen, this));
//   this_thread::sleep_for(chrono::milliseconds(3));
//   if (settings.debug) cout << "Started Hotkey listener..." << endl;
}
bool WaylandHotkeyManager::isListening() {
  return listening;
}
void WaylandHotkeyManager::stopListen() {
//   if (!listening)
//     return;
//   listening = false;
//   // keyPressListener.join();
//   pthread_cancel(keyPressListener.native_handle());
//   if (settings.debug) cout << "Killed keyPressListener..." << endl;
//   // mousePressListener.join();
//   pthread_cancel(mousePressListener.native_handle());
//   if (settings.debug) cout << "Killed mousePressListener..." << endl;
//   for (map<unsigned int, bool>::iterator it = holding_key.begin(); it != holding_key.end(); it++) {
//     it->second = false;
//   }
//   if (settings.debug) cout << "Stopped Hotkey listener." << endl;
}

void WaylandHotkeyManager::mousePressListen() {
  // int fd;
  // struct input_event event;

  // if((fd = open(settings.mouse_file.c_str(), O_RDONLY)) == -1) {
  //   perror("Error while opening device");
  //   exit(EXIT_FAILURE);
  // }
  // while (listening && csgo_.isOnServer()) {
  //   // event value == 1 => button press
  //   // event value == 0 => button release
  //   read(fd, &event, sizeof(struct input_event));
  //   unsigned int keycode = eventCodeToMouseButton(event.code);
  //   // if (settings.debug) cout << "Mouse event:" << keycode << endl;
  //   // skip if no mapping present
  //   if (bindings.find(keycode) == bindings.end()) {
  //     continue;
  //   }
  //   Window activeWin = activeWindow();
  //   if (!holding_key.at(keycode) && event.value > 0 && activeWin == csWindow) { // button press
  //     boost::function<void(unsigned int)> func;
  //     try {
  //       func = bindings.at(keycode);
  //     } catch (const out_of_range& e) {
  //       throw runtime_error("No binding for pressed key. This should not happen.");
  //     }
  //     holding_key[keycode] = true;
  //     threads[keycode] = boost::thread(boost::bind(&WaylandHotkeyManager::callLoop,
  //                                                  this,
  //                                                  keycode,
  //                                                  func));
  //   } else if (holding_key.at(keycode) && event.value == 0) { // button release
  //     holding_key[keycode] = false;
  //     // when debugging wait for thread to terminate
  //     // if (settings.debug) threads[keycode].join();
  //   } else if (activeWin != csWindow) {
  //     if (settings.debug) cout << "Ignoring because other window focused." << endl;
  //   } else {
  //     if (settings.debug) cout << "WARNING: inconsistent mouse state" << endl;
  //     holding_key.at(keycode) = false;
  //   }
  // }
}

