#include "trigger.hpp"
#include "memory_access.hpp"
#include "manager.hpp"
#include "typedef.hpp"

#include <vector>
#include <iostream>

using namespace std;

Trigger::Trigger(GameMananger& csgo) : csgo(csgo) {
  mem = csgo.getMemoryAccess();
}

void Trigger::triggerCheck() {
  vector<EntityType*>& players = csgo.getPlayers();
  Team ownTeam = mem.getTeam();
  unsigned int crosshairTarget = mem.getCrosshairTarget();
  for (EntityType* player : players) {
    if (player->m_iEntityId == crosshairTarget && player->m_iTeamNum != ownTeam)
      cout << "shot fired" << endl;
  }
}
