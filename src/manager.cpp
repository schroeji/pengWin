#include "manager.hpp"
#include "memory_access.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <chrono>
#include <thread>
using namespace std;

GameManager::GameManager(MemoryAccess& mem) : mem(mem) {
  bool fresh_launch = false;
  while (!mem.getPid()) {  // wait until game has launched
    if (!fresh_launch)
      cout << "Waiting for game to launch..." << endl;
    fresh_launch = true;
    this_thread::sleep_for(chrono::milliseconds(1000));
  }
  if(fresh_launch)              // additional wait when game just launched
    this_thread::sleep_for(chrono::milliseconds(15000));
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
  for (EntityType* player : players)
    delete player;
  players.clear();
  local_player_index = -1;
  for (unsigned int i = 0; i < count; i++) {
    // cout << "reading obj: " << i <<endl;
    if (objects[i].m_pEntity == 0)
      continue;

    EntityType* player = new EntityType;
    mem.read(objects[i].m_pEntity, player, sizeof(EntityType));
    if ((player->m_iTeamNum == Team::CT || player->m_iTeamNum == Team::T) && player->m_iHealth > 0 && !player->m_bDormant) {
      // cout << dec << "player: " << players.size() << " addr: " << objects[i].m_pEntity << endl;
      players.push_back(player);
      if (objects[i].m_pEntity == (void*) mem.local_player_addr)
        local_player_index = players.size() - 1;
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
  cout << "---------Players---------" << endl;
  for (EntityType* player : players) {
    cout << dec << "Player: " << i << endl;
    printf("ID: %d\n", player->m_iEntityId);
    cout << "hp: " << player->m_iHealth << endl;
    if(player->m_iTeamNum == Team::CT)
      cout << "Team: CT" << endl;
    else if(player->m_iTeamNum == Team::T)
      cout << "Team: T" << endl;
    printf("Origin x=%f y=%f z=%f\n", player->m_vecOrigin.x, player->m_vecOrigin.y, player->m_vecOrigin.z);
    printf("Angle: x=%f y=%f z=%f\n", player->m_angRotation.x, player->m_angRotation.y, player->m_angRotation.z);
    // printf("dormant2: %x\n", player->__buf_0x11E[3]);
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
  cout << "----------Entities------------" << endl;
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
    cout << "-----" << endl;
  }
}

void GameManager::printPlayerLocationsToFile(const string& filename) {
  if (players.empty())
    return;
  ofstream file;
  file.open(filename);
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

bool GameManager::gameRunning() {
  return mem.getPid() > 0;
}

bool GameManager::isOnServer() {
  mem.updateLocalPlayerAddr();
  return mem.local_player_addr > 0;
}
