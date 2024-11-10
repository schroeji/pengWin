#include "bunnyhop.hpp"
#include "misc/manager.hpp"
#include "misc/settings.hpp"
#include "misc/typedef.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <boost/thread.hpp>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

BunnyHopper::BunnyHopper(GameManager &csgo)
    : csgo(csgo), mem(csgo.getMemoryAccess()),
      settings(Settings::getInstance()) {}

BunnyHopper::~BunnyHopper() {}

void BunnyHopper::jump() {
  // unsigned int jump = 5;
  // mem.write((void*) (mem.force_jump_addr), &jump, sizeof(int));
  // this_thread::sleep_for(chrono::milliseconds(2));
  // jump = 4;
  // mem.write((void*) (mem.force_jump_addr), &jump, sizeof(int));
}

void BunnyHopper::jumpCheck(unsigned int i) {
  // EntityType* localPlayer;
  // bool onGround;
  // try {
  //   localPlayer = csgo.getLocalPlayer();
  // } catch (const exception& e){
  //   return;
  // }
  // onGround = (bool) localPlayer->m_fFlags & FL_ONGROUND;
  // // fix seg faults
  // if (onGround) {
  //   jump();
  // } else {
  //   // cout << "air" << endl;
  // }
}
