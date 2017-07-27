#include "manager.hpp"
#include "memory_access.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

using namespace std;

GameManager::GameManager(MemoryAccess& mem) : mem(mem) {
  if(!mem.getPid())
    exit(0);
  if(!mem.getClientBase()){
    cout << "Could not find Client Base" << endl;
    exit(0);
  }
}

void GameManager::getPlayers(){
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
  for (unsigned int i = 0; i < count; i++) {
    // cout << "reading obj: " << i <<endl;
    EntityType* player = new EntityType;
    mem.read(objects[i].m_pEntity, player, sizeof(EntityType));
    if (player->m_iTeamNum == Teams::CT || player->m_iTeamNum == Teams::T || player->m_iHealth > 0)
      players.push_back(player);
    else
      delete player;
  }
}

void GameManager::printPlayers() {
  int i = 0;
  for (EntityType* player : players) {
    cout << dec << "Player: " << i << endl;
    cout << "hp: " << player->m_iHealth << endl;
    if(player->m_iTeamNum == Teams::CT)
      cout << "Team: CT" << endl;
    else if(player->m_iTeamNum == Teams::T)
      cout << "Team: T" << endl;
    printf("Origin x=%f y=%f z=%f\n", player->m_vecOrigin.x, player->m_vecOrigin.y, player->m_vecOrigin.z);
    printf("ID: %d", player->m_iEntityId);
    i++;
  }
}

void GameManager::printPlayerLocationsToFile(const string& filename) {
  ofstream file;
  file.open(filename);
  if (players.empty())
    return;
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
