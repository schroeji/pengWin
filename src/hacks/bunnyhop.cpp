#include "bunnyhop.hpp"
#include "misc/typedef.hpp"
#include "misc/manager.hpp"
#include "misc/settings.hpp"

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

void BunnyHopper::jumpCheck(unsigned int i) {
  EntityType* localPlayer;
  unsigned int onGround;
  try {
    localPlayer = csgo.getLocalPlayer();
  } catch (exception e){
    return;
  }
  onGround = localPlayer->m_fFlags & 1;
  // fix seg faults
  if (onGround == 1) {
    jump();
  } else {
    // cout << "air" << endl;
  }
}