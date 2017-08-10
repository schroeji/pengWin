#include "manager.hpp"
#include "memory_access.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

using namespace std;

GameManager::GameManager(MemoryAccess& mem) : mem(mem) {
  if (!mem.getPid())
    exit(0);
  if (!mem.getClientRange().first) {
    cout << "Could not find Client Base" << endl;
    exit(0);
  }
  if (!mem.getEngineRange().first){
    cout << "Could not find Engine Base" << endl;
    exit(0);
  }
}

void GameManager::grabPlayers(){
	if (! mem.read((void*) mem.glow_addr, &manager, sizeof(ObjectManager))) {
    cout << "Could not get ObjectManager" << endl;
    return;
  }
	size_t count = manager.objects.Count;
	void* data_ptr = (void*) manager.objects.DataPtr;
  ObjectType objects[1024];
  // cout << hex << "Data : " << data_ptr << endl;
  // cout << dec << "count : " << count << endl;
  if(!mem.read(data_ptr, (void*) objects, sizeof(ObjectType) * count)){
    cout << "Could not get objects" << endl;
    return;
  }
  // cout << "got objects" << endl;
  players.clear();
  local_player_index = -1;
  for (unsigned int i = 0; i < count; i++) {
    // cout << "reading obj: " << i <<endl;
    EntityType* player = new EntityType;
    mem.read(objects[i].m_pEntity, player, sizeof(EntityType));
    if ((player->m_iTeamNum == Team::CT || player->m_iTeamNum == Team::T) && player->m_iHealth > 0) {
      players.push_back(player);
      if (objects[i].m_pEntity == (void*) mem.local_player_addr)
        local_player_index = i;
    }
    else
      delete player;
  }
}

vector<EntityType*>& GameManager::getPlayers() {
  return players;
}

void GameManager::printPlayers() {
  int i = 0;
  cout << "----------Players------------" << endl;
  for (EntityType* player : players) {
    cout << dec << "Player: " << i << endl;
    printf("ID: %d\n", player->m_iEntityId);
    cout << "hp: " << player->m_iHealth << endl;
    if(player->m_iTeamNum == Team::CT)
      cout << "Team: CT" << endl;
    else if(player->m_iTeamNum == Team::T)
      cout << "Team: T" << endl;
    printf("Origin x=%f y=%f z=%f\n", player->m_vecOrigin.x, player->m_vecOrigin.y, player->m_vecOrigin.z);
    // printf("Angle: x=%f y=%f z=%f\n", player->m_angRotation.x, player->m_angRotation.y, player->m_angRotation.z);
    // printf("Angle2: x=%f y=%f z=%f\n", player->m_angAbsRotation.x, player->m_angAbsRotation.y, player->m_angAbsRotation.z);
    // printf("Angle3: x=%f y=%f z=%f\n", player->m_vecAngVelocity.x, player->m_vecAngVelocity.y, player->m_vecAngVelocity.z);
    printf("Angle4: x=%f y=%f z=%f\n", player->m_angNetworkAngles.x, player->m_angNetworkAngles.y, player->m_angNetworkAngles.z);
    cout << "-----" << endl;
    i++;
  }
}

void GameManager::printEntities() {
  if (! mem.read((void*) mem.glow_addr, &manager, sizeof(ObjectManager))) {
    cout << "Could not get ObjectManager" << endl;
    return;
  }
	size_t count = manager.objects.Count;
	void* data_ptr = (void*) manager.objects.DataPtr;
  ObjectType objects[1024];
  // cout << hex << "Data : " << data_ptr << endl;
  // cout << dec << "count : " << count << endl;
  if(!mem.read(data_ptr, (void*) objects, sizeof(ObjectType) * count)){
    cout << "Could not get objects" << endl;
    return;
  }
  cout << "---------Entities---------" << endl;
  for (unsigned int i = 0; i < count; i++) {
    EntityType* entity = new EntityType;
    mem.read(objects[i].m_pEntity, entity, sizeof(EntityType));
    cout << dec << "Nr: " << i << endl;
    printf("ID: %d\n", entity->m_iEntityId);
    cout << "hp: " << entity->m_iHealth << endl;
    // if(player->m_iTeamNum == Team::CT)
      // cout << "Team: CT" << endl;
    // else if(player->m_iTeamNum == Team::T)
      // cout << "Team: T" << endl;
    printf("Origin x=%f y=%f z=%f\n", entity->m_vecOrigin.x, entity->m_vecOrigin.y, entity->m_vecOrigin.z);
    cout << "------" << endl;
    i++;
  }

}

void GameManager::printPlayerLocationsToFile(const string& filename) {
  ofstream file;
  file.open(filename);
  if (players.empty())
    return;
  // print local player index into array[0,0]
  file << local_player_index << ",0,0,0,0,0" << endl;
  // Format: number,hp,team,x,y,z
  int i = 0;
  for (EntityType* player : players) {
    file << i << ",";
    file << player->m_iHealth << ",";
    file << player->m_iTeamNum << ",";
    file << player->m_vecOrigin.x << "," << player->m_vecOrigin.y << "," << player->m_vecOrigin.z << endl;
    i++;
  }
  file.close();
}

MemoryAccess& GameManager::getMemoryAccess() {
  return mem;
}

EntityType* GameManager::getLocalPlayer() {
  if(!players.empty() && local_player_index != -1)
    return players[local_player_index];
  else
    return nullptr;
}
