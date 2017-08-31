#include "trigger.hpp"
#include "memory_access.hpp"
#include "manager.hpp"
#include "clicker.hpp"
#include "typedef.hpp"
#include "settings.hpp"

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <X11/keysymdef.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <boost/thread.hpp>

using namespace std;

Trigger::Trigger(GameManager& csgo) : csgo(csgo),
                                      mem(csgo.getMemoryAccess()),
                                      clicker(Clicker(csgo.getMemoryAccess())),
                                      settings(Settings::getInstance()){
  display = XOpenDisplay(NULL);
  rootWindow = DefaultRootWindow(display);
  keycode = XKeysymToKeycode(display, settings.trigger_key);
}

Trigger::~Trigger() {
  XCloseDisplay(display);
}

void Trigger::triggerLoop() {
  unsigned int modifiers = AnyModifier;
  int pointer_mode = GrabModeAsync;
  int keyboard_mode = GrabModeAsync;
  bool owner_events = false;
  XEvent event;
  if (settings.debug) cout << "Entered Triggerloop" << endl;
  XUngrabKey(display, keycode, modifiers, rootWindow);
  XGrabKey(display, keycode, modifiers, rootWindow, owner_events, pointer_mode,
           keyboard_mode);
  XSelectInput(display, rootWindow, KeyPressMask);
  while (csgo.isOnServer()) {
    XNextEvent(display, &event);
    if (event.type == KeyPress) {
      triggerThread = boost::thread(boost::bind(&Trigger::triggerCheck, this));
      holding_hotkey = true;
      if (settings.debug) cout << "Trigger Hotkey pressed." << endl;
    }
    while (holding_hotkey) {
      XNextEvent(display, &event);
      if(event.type == KeyRelease && event.xbutton.button == keycode) {
        holding_hotkey = false;
        triggerThread.join();
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
  XUngrabKey(display, keycode, modifiers, rootWindow);
}

void Trigger::triggerCheck() {
  if (settings.debug) cout << "triggerCheck running..." << endl;
  while (holding_hotkey) {
    vector<EntityType*>& players = csgo.getPlayers();
    if (players.size() < 2)
      continue;
    Team ownTeam = mem.getTeam();
    unsigned int crosshairTarget = mem.getCrosshairTarget();
    if (settings.debug) cout << "crosshairTarget: " << crosshairTarget << endl;
    if (!crosshairTarget)
      continue;

    normal_distribution<double> distrib((double) settings.trigger_delay, (double) settings.trigger_delay / 2);
    std::default_random_engine gen;
    for (EntityType* player : players) {
      if (player->m_iEntityId == crosshairTarget && player->m_iTeamNum != ownTeam){
        if (settings.trigger_use_random) {
          long int wait_time = (long) distrib(gen);
          while (wait_time < 0)
            wait_time = (long) distrib(gen);
          this_thread::sleep_for(chrono::milliseconds(wait_time));
        }
        else {
          this_thread::sleep_for(chrono::milliseconds(settings.trigger_delay));
        }
        // clicker.memClick();
        if (settings.debug) cout << "fired shot" << endl;
        clicker.xClick();
      }
    }
    this_thread::sleep_for(chrono::milliseconds(1));
  }
  if (settings.debug) cout << "triggerCheck ended..." << endl;
}
