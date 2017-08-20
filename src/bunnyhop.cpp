#include "bunnyhop.hpp"
#include "typedef.hpp"
#include "manager.hpp"

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

BunnyHopper::BunnyHopper(GameManager& csgo) :
  csgo(csgo),
  mem(csgo.getMemoryAccess()){}

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
