#include "trigger.hpp"
#include "memory_access.hpp"
#include "manager.hpp"
#include "clicker.hpp"
#include "typedef.hpp"

#include <vector>
#include <iostream>

using namespace std;

Trigger::Trigger(GameManager& csgo) : csgo(csgo),
                                      mem(csgo.getMemoryAccess()),
                                      clicker(Clicker(csgo.getMemoryAccess())) {
}

void Trigger::triggerCheck() {
  vector<EntityType*>& players = csgo.getPlayers();
  Team ownTeam = mem.getTeam();
  unsigned int crosshairTarget = mem.getCrosshairTarget();
  if(!crosshairTarget)
    return;
  for (EntityType* player : players) {
    if (player->m_iEntityId == crosshairTarget && player->m_iTeamNum != ownTeam){
      // clicker.memClick();
      clicker.xClick();
    }
  }
}
