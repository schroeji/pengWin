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

#define hotkeycode XK_Caps_Lock

using namespace std;

Trigger::Trigger(GameManager& csgo) : csgo(csgo),
                                      mem(csgo.getMemoryAccess()),
                                      clicker(Clicker(csgo.getMemoryAccess())),
                                      settings(Settings::getInstance()){
  display = XOpenDisplay(NULL);
  rootWindow = DefaultRootWindow(display);
  unsigned int modifiers = AnyModifier;
  int keycode = XKeysymToKeycode(display, hotkeycode);
  int pointer_mode = GrabModeAsync;
  int keyboard_mode = GrabModeAsync;
  bool owner_events = False;
  XUngrabKey(display, keycode, modifiers, rootWindow);
  XGrabKey(display, keycode, modifiers, rootWindow, owner_events, pointer_mode,
           keyboard_mode);
}

void Trigger::triggerCheck() {
  vector<EntityType*>& players = csgo.getPlayers();
  Team ownTeam = mem.getTeam();
  unsigned int crosshairTarget = mem.getCrosshairTarget();
  if(!crosshairTarget)
    return;
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
      clicker.xClick();
    }
  }
}
