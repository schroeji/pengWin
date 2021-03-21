#include "manager.hpp"
#include "memory_access.hpp"
#include "util.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <thread>

using namespace std;

GameManager::GameManager(MemoryAccess &mem)
    : mem(mem), settings(Settings::getInstance()) {
  bool fresh_launch = false;
  while (!mem.getPid()) { // wait until game has launched
    if (!fresh_launch)
      cout << "Waiting for game to launch..." << endl;
    fresh_launch = true;
    this_thread::sleep_for(chrono::milliseconds(1000));
  }
  if (fresh_launch) // additional wait when game just launched
    this_thread::sleep_for(chrono::milliseconds(15000));
  if (!mem.getClientRange().first) {
    cout << "Could not find Client Base" << endl;
    exit(0);
  }
  if (!mem.getEngineRange().first) {
    cout << "Could not find Engine Base" << endl;
    exit(0);
  }
}

void GameManager::grabPlayers() {
  if (!mem.read((void *)mem.glow_addr, &manager, sizeof(GlowObjectManager_t))) {
    cout << "Could not get GlowObjectManager" << endl;
    return;
  }
  size_t count = manager.objects.Count;
  void *data_ptr = (void *)manager.objects.DataPtr;
  // cout << hex << "Data : " << data_ptr << endl;
  // if (settings.debug) cout << dec << "object count: " << count << endl;
  if (!mem.read(data_ptr, (void *)g_glow,
                sizeof(GlowObjectDefinition_t) * count)) {
    cout << "Could not get objects" << endl;
    return;
  }
  vector<EntityType *> new_players;
  vector<EntityType *> new_nonPlayerEntities;
  vector<addr_type> new_player_addrs;
  local_player_index = -1;
  for (unsigned int i = 0; i < count; i++) {
    // cout << "reading obj: " << i <<endl;
    if (g_glow[i].m_pEntity == 0)
      continue;

    EntityType *entity = new EntityType;
    mem.read(g_glow[i].m_pEntity, entity, sizeof(EntityType));
    if ((entity->m_iTeamNum == Team::CT || entity->m_iTeamNum == Team::T) &&
        entity->m_iHealth > 0) {
      // cout << dec << "player: " << new_players.size() << " addr: " <<
      // objects[i].m_pEntity << endl;
      new_players.push_back(entity);
      new_player_addrs.push_back((addr_type)g_glow[i].m_pEntity);
      if (g_glow[i].m_pEntity == (void *)mem.local_player_addr)
        local_player_index = new_players.size() - 1;
    } else
      new_nonPlayerEntities.push_back(entity);
  }
  // copy new players and delete old ones afterwards for thread safety
  vector<EntityType *> old_players = players;
  players = new_players;
  player_addrs = new_player_addrs;
  nonPlayerEntities = new_nonPlayerEntities;
  for (EntityType *player : old_players)
    delete player;
  for (EntityType *ent : new_nonPlayerEntities)
    delete ent;

  // refresh helper variables
  if (isFlashed(mem.local_player_addr))
    local_player_flashed_timer += float(settings.main_loop_sleep) / 1000.;
  else
    local_player_flashed_timer = 0.0;
}

vector<EntityType *> &GameManager::getPlayers() { return players; }

void GameManager::printPlayers() {
  int i = 0;
  cout << "---------Players---------" << endl;
  for (EntityType *player : players) {
    cout << dec << "--- Player: " << i << "---" << endl;
    cout << hex << "Addr: " << player_addrs[i] << endl;
    printf("ID: %d\n", player->m_iEntityId);
    cout << dec << "hp: " << player->m_iHealth << endl;
    if (player->m_iTeamNum == Team::CT)
      cout << "Team: CT" << endl;
    else if (player->m_iTeamNum == Team::T)
      cout << "Team: T" << endl;
    printf("Origin x=%f y=%f z=%f\n", player->m_vecOrigin.x,
           player->m_vecOrigin.y, player->m_vecOrigin.z);
    // printf("Angle: x=%4.16lf y=%4.16lf z=%f\n", player->m_angAbsRotation.x,
    // player->m_angAbsRotation.y, player->m_angNetworkAngles.z);
    printf("Angle: x=%4.16lf y=%4.16lf z=%f\n", player->m_angNetworkAngles.x,
           player->m_angNetworkAngles.y, player->m_angNetworkAngles.z);
    printf("view offset: %f, %f\n", player->m_vecViewOffset.x,
           player->m_vecViewOffset.y);
    printf("m_fFlags: %u\n", player->m_fFlags);
    printf("Velocity: %f, %f, %f\n", player->m_vecVelocity.x,
           player->m_vecVelocity.y, player->m_vecVelocity.z);
    printf("Aimpunch: %f, %f, %f\n", getAimPunch(mem.local_player_addr).x,
           getAimPunch(mem.local_player_addr).y,
           getAimPunch(mem.local_player_addr).z);
    printf("Defusing: %d\n", isDefusing(player_addrs[i]));
    printf("Flashed: %d\n", isFlashed(player_addrs[i]));
    printf("Weapon: %s\n", getWeaponName(getWeapon(player_addrs[i])).c_str());
    printf("dormant: %u\n", player->m_bDormant);
    // vector<int> diffs = mem.diffMem(mem.local_player_addr + 0x3600, 0x300);
    // if (diffs.size() > 0) {
    //   for (int j : diffs)
    //     cout << hex << j << endl;
    //   mem.printBlock(mem.local_player_addr + 0x3600, 0x300);
    // }
    cout << "-----" << endl;
    i++;
  }
}

void GameManager::printEntities() {
  if (!mem.read((void *)mem.glow_addr, &manager, sizeof(GlowObjectManager_t))) {
    cout << "Could not get GlowObjectManager" << endl;
    return;
  }
  size_t count = manager.objects.Count;
  void *data_ptr = (void *)manager.objects.DataPtr;
  // cout << hex << "Data : " << data_ptr << endl;
  // cout << dec << "count : " << count << endl;
  if (!mem.read(data_ptr, (void *)g_glow,
                sizeof(GlowObjectDefinition_t) * count)) {
    cout << "Could not get objects" << endl;
    return;
  }
  cout << "----------Entities------------" << endl;
  for (unsigned int i = 0; i < count; i++) {
    EntityType *entity = new EntityType;
    if (g_glow[i].m_pEntity == nullptr)
      continue;
    cout << dec << "Nr: " << i << endl;
    cout << hex << "Addr:" << g_glow[i].m_pEntity << endl;
    mem.read(g_glow[i].m_pEntity, entity, sizeof(EntityType));
    printf("ID: %d\n", entity->m_iEntityId);
    cout << "hp: " << entity->m_iHealth << endl;
    printf("Origin x=%f y=%f z=%f\n", entity->m_vecOrigin.x,
           entity->m_vecOrigin.y, entity->m_vecOrigin.z);
    printf("m_fFlags: %u\n", entity->m_fFlags);
    printf("m_iEFlags: %d\n", entity->m_iEFlags);
    vector<int> diffs = mem.diffMem((addr_type)g_glow[i].m_pEntity, 0x200);
    if (diffs.size() > 0) {
      // for (int i : diffs)
      // cout << hex << i << endl;
      mem.printBlock((addr_type)g_glow[i].m_pEntity, 0x200);
    }
    cout << "-----" << endl;
  }
}

void GameManager::printPlayerLocationsToFile(const string &filename) {
  if (players.empty())
    return;
  ofstream file;
  file.open(filename);
  // print local player index into array[0,0]
  file << local_player_index << ",0,0,0,0,0" << endl;
  // Format: number,hp,team,x,y,z
  int i = 0;
  for (EntityType *player : players) {
    file << i << ",";
    file << player->m_iHealth << ",";
    file << player->m_iTeamNum << ",";
    file << player->m_vecOrigin.x << "," << player->m_vecOrigin.y << ","
         << player->m_vecOrigin.z << endl;
    i++;
  }
  file.close();
}

MemoryAccess &GameManager::getMemoryAccess() { return mem; }

EntityType *GameManager::getLocalPlayer() {
  mem.updateLocalPlayerAddr();
  if (!mem.read((void *)mem.local_player_addr, (void *)local_player,
                sizeof(EntityType))) {
    connected = false;
    throw runtime_error("No local player");
  }
  connected = true;
  return local_player;
}

int GameManager::getLocalPlayerIndex() { return local_player_index; }

bool GameManager::gameRunning() { return mem.getPid() > 0; }

bool GameManager::isOnServer() {
  if (!connected) {
    try {
      getLocalPlayer();
    } catch (const runtime_error &e) {
      return false;
    }
  }
  return connected;
}

addr_type GameManager::getPlayerAddr(EntityType *player) {
  for (unsigned int i = 0; i < players.size(); i++) {
    if (player == players[i])
      return player_addrs[i];
  }
  return 0;
}

string GameManager::getMapName() {
  if (current_map_ == "") {
    if (settings.find_map) {
      if (settings.debug)
        cout << "Scanning for map..." << endl;
      if (settings.radar_generic)
        current_map_ = "generic";
      while (current_map_ == "") {
        char MapName[32];
        if (!mem.read((void *)(mem.map_name_addr), &MapName, sizeof(MapName)))
          throw runtime_error("Could not get MapName.");
        // mem.read((void*)(Address + OFFSET_MAPNAME), &MapName,
        // sizeof(MapName));
        string map_path(MapName);
        // vector<string> no_path = split_string(map_path, "/");
        vector<string> no_bsp = split_string(map_path, ".");
        current_map_ = string(no_bsp[0]);
        this_thread::sleep_for(chrono::milliseconds(1000));
      }
      cout << "Found Map: " << current_map_ << endl;
    } else {
      cout << "Map detection deactivated. Please choose map:" << endl;
      cin >> current_map_;
    }
  }
  return current_map_;
}

bool GameManager::isScoped(addr_type player_addr) {
  char buf;
  if (!mem.read((void *)(player_addr + mem.m_bIsScoped), &buf, sizeof(buf)))
    return false;
  return (bool)buf;
}

Team GameManager::getTeam(addr_type player_addr) {
  unsigned int team;
  if (!mem.read((void *)(player_addr + mem.m_iTeamNum), &team, sizeof(team)))
    throw runtime_error("Could not get Team.");
  return (Team)team;
}

unsigned int GameManager::getCrosshairTarget(addr_type player_addr) {
  unsigned int target;
  if (!mem.read((void *)(player_addr + mem.m_iCrosshairIndex), &target,
                sizeof(target)))
    throw runtime_error("Could not get CrosshairTarget.");
  return target;
}

QAngle GameManager::getAimPunch(addr_type player_addr) {
  QAngle ang;
  if (!mem.read((void *)(player_addr + mem.m_Local + mem.m_aimPunchAngle), &ang,
                sizeof(ang)))
    throw runtime_error("Could not get AimPunch.");
  return ang;
}

bool GameManager::isDefusing(addr_type player_addr) {
  char buf;
  if (!mem.read((void *)(player_addr + mem.m_bIsDefusing), &buf, sizeof(buf)))
    return false;
  return (bool)buf;
}

bool GameManager::isFlashed(addr_type player_addr) {
  return getFlashDuration(player_addr) > 0.0;
}

float GameManager::getFlashDuration(addr_type player_addr) {
  float buf;
  if (!mem.read((void *)(player_addr + mem.m_flFlashDuration), &buf,
                sizeof(buf)))
    return 0.0;
  return buf;
}

QAngle GameManager::getNetworkAngles(addr_type player_addr) {
  QAngle ang;
  if (!mem.read((void *)(player_addr + 0x164), &ang, sizeof(ang)))
    throw runtime_error("Could not get NetworkAngles.");
  return ang;
}

Weapon GameManager::getWeapon(addr_type player_addr) {
  unsigned int activeWeaponID;
  if (!mem.read((void *)(player_addr + mem.m_hActiveWeapon), &activeWeaponID,
                sizeof(int)))
    return Weapon::NONE;
  // if(settings.debug) cout << "Active weaponID:" << activeWeaponID << endl;
  activeWeaponID &= 0xFFF;
  unsigned int weaponID = 0;
  EntityType currentEntity;
  for (size_t i = 0; i < manager.objects.Count; i++) {
    if (g_glow[i].m_pEntity == nullptr)
      continue;
    if (!mem.read(g_glow[i].m_pEntity, &currentEntity, sizeof(EntityType)))
      continue;
    if (currentEntity.m_iEntityId ==
        activeWeaponID) { // found entity for weapon
      // if(settings.debug) cout << "Found entity" << endl;
      // get weapon type
      mem.read((void *)((addr_type)g_glow[i].m_pEntity +
                        mem.m_AttributeManager + 0x60 +
                        mem.m_iItemDefinitionIndex),
               &weaponID, sizeof(int));
      weaponID &= 0xFFF;
      // if(settings.debug) cout << "weaponID:" << weaponID << endl;
      break;
    }
  }
  return (Weapon)weaponID;
}

std::vector<Vector> GameManager::getSmokeLocations() {
  // super hacky way to detect smokes from the entity list:
  // if a non player entity has FL_ONGROUND set it's a smoke
  std::vector<Vector> result;
  for (EntityType *entity : nonPlayerEntities) {
    if (!(entity->m_fFlags & FL_CLIENT) && (entity->m_fFlags & FL_ONGROUND)) {
      result.push_back(entity->m_vecOrigin);
    }
  }
  return result;
}

bool GameManager::lineThroughSmoke(Vector start, Vector end) {
  std::vector<Vector> smokeLocations = getSmokeLocations();
  Vector d = end - start;
  normalize_vector(&d);
  for (Vector smoke : smokeLocations) {
    if (lineSphereIntersection(start, end, smoke, smokeRadius))
      return true;
  }
  return false;
}

bool GameManager::isLocalPlayerFlashed() {
  return getFlashDuration(mem.local_player_addr) - local_player_flashed_timer >
         FLASH_END_OFFSET;
};
