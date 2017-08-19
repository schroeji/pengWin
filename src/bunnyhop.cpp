#include "bunnyhop.hpp"
#include "typedef.hpp"
#include "manager.hpp"
#include "settings.hpp"

#include <X11/keysymdef.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <boost/thread.hpp>

using namespace std;

BunnyHopper::BunnyHopper(GameManager& csgo) : csgo(csgo),
                                             mem(csgo.getMemoryAccess()),
                                             settings(Settings::getInstance()) {
  display = XOpenDisplay(NULL);
  rootWindow = DefaultRootWindow(display);
  keycode = XKeysymToKeycode(display, XK_space);
}

BunnyHopper::~BunnyHopper(){
  XCloseDisplay(display);
}

void BunnyHopper::jump() {
  unsigned int jump = 5;
  mem.write((void*) (mem.force_jump_addr), &jump, sizeof(int));
  this_thread::sleep_for(chrono::milliseconds(2));
  jump = 4;
  mem.write((void*) (mem.force_jump_addr), &jump, sizeof(int));
}

void BunnyHopper::jumpCheck() {
  EntityType* localPlayer = csgo.getLocalPlayer();
  if (!localPlayer)
    return;
  unsigned int onGround = localPlayer->m_fFlags & 1;
  if (onGround == 1) {
    jump();
  }
}

void BunnyHopper::jumpLoop() {
  unsigned int modifiers = AnyModifier;
  int pointer_mode = GrabModeAsync;
  int keyboard_mode = GrabModeAsync;
  bool owner_events = false;
  XEvent event;
  if (settings.debug) cout << "Entered bhopLoop" << endl;
  XUngrabKey(display, keycode, modifiers, rootWindow);
  XGrabKey(display, keycode, modifiers, rootWindow, owner_events, pointer_mode,
           keyboard_mode);
  XSelectInput(display, rootWindow, KeyPressMask);
  while (true) {
    XNextEvent(display, &event);
    if (event.type == KeyPress) {
      jumpThread = boost::thread(boost::bind(&BunnyHopper::jumpCheck, this));
      holding_hotkey = true;
      if (settings.debug) cout << "Bhop Hotkey pressed." << endl;
    }
    while (holding_hotkey) {
      XNextEvent(display, &event);
      if(event.type == KeyRelease && event.xbutton.button == keycode) {
        holding_hotkey = false;
        jumpThread.join();
        if (settings.debug) cout << "Thread joined" << endl;
      } else {
        // XTestFakeKeyEvent(display, event.xbutton.button, event.type == KeyPress, 0);
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
    }
  }
}
