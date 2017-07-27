#include "trigger.hpp"
#include "memory_access.hpp"
#include "manager.hpp"
#include "typedef.hpp"

#include <vector>
#include <iostream>

using namespace std;

Trigger::Trigger(GameManager& csgo) : csgo(csgo),
                                     mem(csgo.getMemoryAccess()) {}

void Trigger::triggerCheck() {
  vector<EntityType*>& players = csgo.getPlayers();
  Team ownTeam = mem.getTeam();
  cout << "Team:" << ownTeam << endl;
  unsigned int crosshairTarget = mem.getCrosshairTarget();
  cout << "crosshairtarget: " << crosshairTarget << endl;
  if(!crosshairTarget)
    return;
  for (EntityType* player : players) {
    cout << "player ID: " << player->m_iEntityId << endl;
    if (player->m_iEntityId == crosshairTarget && player->m_iTeamNum != ownTeam)
      cout << "shot fired" << endl;
  }
}
