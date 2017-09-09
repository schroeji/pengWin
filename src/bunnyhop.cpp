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
}

BunnyHopper::~BunnyHopper(){
}

void BunnyHopper::jump() {
  unsigned int jump = 5;
  mem.write((void*) (mem.force_jump_addr), &jump, sizeof(int));
  this_thread::sleep_for(chrono::milliseconds(2));
  jump = 4;
  mem.write((void*) (mem.force_jump_addr), &jump, sizeof(int));
}

void BunnyHopper::jumpCheck() {
  EntityType* localPlayer;
  try {
    localPlayer = csgo.getLocalPlayer();
  } catch (runtime_error e){
    return;
  }
  // fix seg faults
  unsigned int onGround = localPlayer->m_fFlags & 1;
  if (onGround == 1) {
    // cout << "jump" << i << endl;
    jump();
  } else {
    // cout << "air" << endl;
  }
}
