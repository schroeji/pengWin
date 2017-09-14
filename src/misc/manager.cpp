#include "manager.hpp"
#include "memory_access.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <stdexcept>

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
  vector<EntityType*> new_players;
  vector<addr_type> new_player_addrs;
  local_player_index = -1;
  for (unsigned int i = 0; i < count; i++) {
    // cout << "reading obj: " << i <<endl;
    if (objects[i].m_pEntity == 0)
      continue;

    EntityType* player = new EntityType;
    mem.read(objects[i].m_pEntity, player, sizeof(EntityType));
    if ((player->m_iTeamNum == Team::CT || player->m_iTeamNum == Team::T) && player->m_iHealth > 0 && !player->m_bDormant) {
      // cout << dec << "player: " << new_players.size() << " addr: " << objects[i].m_pEntity << endl;
      new_players.push_back(player);
      new_player_addrs.push_back((addr_type) objects[i].m_pEntity);
      if (objects[i].m_pEntity == (void*) mem.local_player_addr)
        local_player_index = new_players.size() - 1;
    }
    else
      delete player;
  }
  // copy new players and delete old ones after for thread safety
  vector<EntityType*> old_players = players;
  players = new_players;
  player_addrs = new_player_addrs;
  for (EntityType* player : old_players)
    delete player;
}

vector<EntityType*>& GameManager::getPlayers() {
  return players;
}

void GameManager::printPlayers() {
  int i = 0;
  cout << "---------Players---------" << endl;
  for (EntityType* player : players) {
    cout << dec << "Player: " << i << endl;
    cout << hex << "Addr: " << player_addrs[i] << endl;
    printf("ID: %d\n", player->m_iEntityId);
    cout << dec << "hp: " << player->m_iHealth << endl;
    if(player->m_iTeamNum == Team::CT)
      cout << "Team: CT" << endl;
    else if(player->m_iTeamNum == Team::T)
      cout << "Team: T" << endl;
    printf("Origin x=%f y=%f z=%f\n", player->m_vecOrigin.x, player->m_vecOrigin.y, player->m_vecOrigin.z);
    printf("Angle: x=%4.16lf y=%4.16lf z=%f\n", player->m_angNetworkAngles.x, player->m_angNetworkAngles.y, player->m_angNetworkAngles.z);
    printf("view offset: %f, %f\n", player->m_vecViewOffset.x,  player->m_vecViewOffset.y);
    printf("Velocity: %f, %f, %f\n", player->m_vecVelocity.x,  player->m_vecVelocity.y, player->m_vecVelocity.z);
    printf("Aimpunch: %f, %f, %f\n", getAimPunch().x,  getAimPunch().y, getAimPunch().z);
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
  mem.updateLocalPlayerAddr();
  if (!mem.read((void*) mem.local_player_addr, (void*) local_player, sizeof(EntityType)))
      throw runtime_error("No local player");
  return local_player;

  // if(!players.empty() && local_player_index != -1)
  //   return players[local_player_index];
  // else
  //   throw runtime_error("No local player");
}

bool GameManager::gameRunning() {
  return mem.getPid() > 0;
}

bool GameManager::isOnServer() {
  mem.updateLocalPlayerAddr();
  return mem.local_player_addr > 0;
}

addr_type GameManager::getPlayerAddr(EntityType* player) {
  for (unsigned int i = 0; i < players.size(); i++) {
    if (player == players[i])
      return player_addrs[i];
  }
  return 0;
}

string GameManager::getMapName() {
  char MapName[32];
  if (!mem.read((void*)(mem.map_name_addr), &MapName, sizeof(MapName)))
    throw runtime_error("Could not get MapName.");
  // mem.read((void*)(Address + OFFSET_MAPNAME), &MapName, sizeof(MapName));
  string map_path(MapName);
  // vector<string> no_path = split_string(map_path, "/");
  vector<string> no_bsp = split_string(map_path, ".");
  string ret(no_bsp[0]);
  return ret;
}

bool GameManager::isScoped() {
  char buf;
  if(!mem.read((void*) (mem.local_player_addr + mem.m_bIsScoped), &buf, sizeof(buf)))
     return false;
  return (bool) buf;
}

Team GameManager::getTeam() {
  unsigned int team;
  if(!mem.read((void*) (mem.local_player_addr + mem.m_iTeamNum), &team, sizeof(team)))
    throw runtime_error("Could not get Team.");
  return (Team) team;
}

unsigned int GameManager::getCrosshairTarget() {
  unsigned int ret;
  if(!mem.read((void*) (mem.local_player_addr + mem.m_iCrosshairIndex), &ret, sizeof(ret)))
    throw runtime_error("Could not get CrosshairTarget.");
  return ret;
}

QAngle GameManager::getAimPunch() {
  QAngle ang;
  if(!mem.read((void*) (mem.local_player_addr + mem.m_Local + mem.m_aimPunchAngle), &ang, sizeof(ang)))
    throw runtime_error("Could not get CrosshairTarget.");
  return ang;
}
