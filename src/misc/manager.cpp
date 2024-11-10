#include "manager.hpp"
#include "memory_access.hpp"
#include "typedef.hpp"
#include "util.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <thread>

using namespace std;

GameManager::GameManager(MemoryAccess &mem)
    : mem(mem), pattern_scanner(mem), settings(Settings::getInstance()) {
  bool fresh_launch = false;
  while (!mem.getPid()) { // wait until game has launched
    if (!fresh_launch)
      cout << "Waiting for game to launch..." << endl;
    fresh_launch = true;
    this_thread::sleep_for(chrono::milliseconds(1000));
  }
  if (fresh_launch) // additional wait when game just launched
    this_thread::sleep_for(chrono::milliseconds(15000));
  if (mem.getClientRange().empty()) {
    cout << "Could not find Client Base" << endl;
    exit(0);
  }
  if (mem.getEngineRange().empty()) {
    cout << "Could not find Engine Base" << endl;
    exit(0);
  }
}

void GameManager::grabPlayers() {
  addr_type entity_list_addr{pattern_scanner.getEntitySystem() +
                             pattern_scanner.getEntityListOffset()};
  if (!mem.read((void *)entity_list_addr, &entity_list,
                sizeof(CConcreteEntityList))) {
    cout << "Could not get EntityList" << endl;
    return;
  }
  addr_type entity_list_max_index_addr{
      pattern_scanner.getEntitySystem() +
      pattern_scanner.getEntityListMaxIndexOffset()};
  auto entity_list_max_index =
      mem.read_uint32((void *)entity_list_max_index_addr);
  addr_type local_player_pawn_addr{pattern_scanner.getLocalPlayerController() +
                                   pattern_scanner.getPawnHandleOffset()};
  auto local_player_pawn_handle =
      mem.read_uint32((void *)local_player_pawn_addr);
  auto local_player_pawn_index = local_player_pawn_handle & 0x7FFF;
  vector<PlayerPtr> new_players;
  vector<CEntityInstance *> new_nonPlayerEntities;
  vector<addr_type> new_player_addrs;
  local_player_index = -1;

  for (int i = 0; i <= entity_list_max_index; ++i) {
    const auto chunkIndex =
        i / CConcreteEntityList::kNumberOfIdentitiesPerChunk;
    addr_type chunk = (addr_type)entity_list.chunks[chunkIndex];
    if (!chunk)
      continue;

    const auto indexInChunk =
        i % CConcreteEntityList::kNumberOfIdentitiesPerChunk;
    CConcreteEntityList::EntityChunk entity_chunk{};
    mem.read(chunk, &entity_chunk, sizeof(entity_chunk));
    const auto &entityIdentity = entity_chunk[indexInChunk];
    if (entityIdentity.entity && (entityIdentity.handle & 0x7fff) == i) {
      addr_type entity_health_addr{(addr_type)entityIdentity.entity +
                                   pattern_scanner.getHealthOffset()};
      auto health = mem.read_uint32((void *)entity_health_addr);
      if (health > 0) {
        auto player = makePlayerPtr((addr_type)entityIdentity.entity);
        new_players.push_back(player);
        if (i == local_player_pawn_index)
          local_player_index = new_players.size() - 1;
      }
    }
    vector<PlayerPtr> old_players = players;
    players = new_players;
    for (CEntityInstance *ent : new_nonPlayerEntities)
      delete ent;
  }

  // for (unsigned int i = 0; i < count; i++) {
  //   // cout << "reading obj: " << i <<endl;
  //   if (g_glow[i].m_pEntity == 0)
  //     continue;

  //   EntityType *entity = new EntityType;
  //   mem.read(g_glow[i].m_pEntity, entity, sizeof(EntityType));
  //   if ((entity->m_iTeamNum == Team::CT || entity->m_iTeamNum == Team::T) &&
  //       entity->m_iHealth > 0) {
  //     // cout << dec << "player: " << new_players.size() << " addr: " <<
  //     // objects[i].m_pEntity << endl;
  //     new_players.push_back(entity);
  //     new_player_addrs.push_back((addr_type)g_glow[i].m_pEntity);
  //     if (g_glow[i].m_pEntity == (void *)mem.local_player_addr)
  //       local_player_index = new_players.size() - 1;
  //   } else
  //     new_nonPlayerEntities.push_back(entity);
  // }
  // // copy new players and delete old ones afterwards for thread safety
  // vector<EntityType *> old_players = players;
  // players = new_players;
  // player_addrs = new_player_addrs;
  // nonPlayerEntities = new_nonPlayerEntities;
  // for (EntityType *player : old_players)
  //   delete player;
  // for (EntityType *ent : new_nonPlayerEntities)
  //   delete ent;

  // // refresh helper variables
  // if (isFlashed(mem.local_player_addr))
  //   local_player_flashed_timer += float(settings.main_loop_sleep) / 1000.;
  // else
  //   local_player_flashed_timer = 0.0;
}

vector<PlayerPtr> &GameManager::getPlayers() { return players; }

void GameManager::printPlayers() {
  int i = 0;
  cout << "---------Players---------" << endl;
  for (auto player : players) {
    cout << dec << "--- Player: " << i << "---" << endl;
    cout << hex << "Addr: " << player->entity_addr << endl;
    cout << dec << "hp: " << player->health << endl;
    if (player->team == Team::CT)
      cout << "Team: CT" << endl;
    else if (player->team == Team::T)
      cout << "Team: T" << endl;
    cout << "Defusing: " << player->is_defusing << endl;
    cout << "Weapon: " << player->weapon << endl;
    printVec("Origin:", player->origin);
    printVec("View Offset:", player->viewOrigin);
    printVec("ViewAngles:", player->networkAngle);
    printVec("AimPunch:", player->aimPunch);

    // printf("Angle: x=%4.16lf y=%4.16lf z=%f\n", player->m_angAbsRotation.x,
    // player->m_angAbsRotation.y, player->m_angNetworkAngles.z);
    // printf("Angle: x=%4.16lf y=%4.16lf z=%f\n", player->m_angNetworkAngles.x,
    //        player->m_angNetworkAngles.y, player->m_angNetworkAngles.z);
    // printf("view offset: %f, %f\n", player->m_vecViewOffset.x,
    //        player->m_vecViewOffset.y);
    // printf("m_fFlags: %u\n", player->m_fFlags);
    // printf("Velocity: %f, %f, %f\n", player->m_vecVelocity.x,
    //        player->m_vecVelocity.y, player->m_vecVelocity.z);
    // printf("Aimpunch: %f, %f, %f\n", getAimPunch(mem.local_player_addr).x,
    //        getAimPunch(mem.local_player_addr).y,
    //        getAimPunch(mem.local_player_addr).z);
    // printf("Defusing: %d\n", isDefusing(player_addrs[i]));
    // printf("Flashed: %d\n", isFlashed(player_addrs[i]));
    // printf("Weapon: %s\n",
    // getWeaponName(getWeapon(player_addrs[i])).c_str()); printf("dormant:
    // %u\n", player->m_bDormant); vector<int> diffs =
    // mem.diffMem(mem.local_player_addr + 0x3600, 0x300); if (diffs.size() > 0)
    // {
    //   for (int j : diffs)
    //     cout << hex << j << endl;
    //   mem.printBlock(mem.local_player_addr + 0x3600, 0x300);
    // }
    cout << "-----" << endl;
    i++;
  }
}

void GameManager::printEntities() {
  addr_type entity_list_addr{pattern_scanner.getEntitySystem() +
                             pattern_scanner.getEntityListOffset()};
  if (!mem.read((void *)entity_list_addr, &entity_list,
                sizeof(CConcreteEntityList))) {
    cout << "Could not get EntityList" << endl;
    return;
  }
  addr_type entity_list_max_index_addr{
      pattern_scanner.getEntitySystem() +
      pattern_scanner.getEntityListMaxIndexOffset()};
  auto entity_list_max_index =
      mem.read_uint32((void *)entity_list_max_index_addr);

  cout << "----------Entities------------" << endl;
  for (int i = 0; i <= entity_list_max_index; ++i) {
    const auto chunkIndex =
        i / CConcreteEntityList::kNumberOfIdentitiesPerChunk;
    void *chunk = entity_list.chunks[chunkIndex];
    if (!chunk)
      continue;

    const auto indexInChunk =
        i % CConcreteEntityList::kNumberOfIdentitiesPerChunk;
    CConcreteEntityList::EntityChunk entity_chunk{};
    mem.read(chunk, &entity_chunk, sizeof(entity_chunk));
    const auto &entityIdentity = entity_chunk[indexInChunk];
    if (entityIdentity.entity && (entityIdentity.handle & 0x7fff) == i) {
      cout << "Found entity:" << i << endl;
      addr_type entity_health_addr{(addr_type)entityIdentity.entity +
                                   pattern_scanner.getHealthOffset()};
      addr_type entity_team_addr{(addr_type)entityIdentity.entity +
                                 pattern_scanner.getTeamNumberOffset()};
      auto health = mem.read_uint32((void *)entity_health_addr);
      auto team = mem.read_uint8((void *)entity_team_addr);
      cout << "Team: " << team << endl;
      cout << "Health: " << health << endl;
    }
  }

  // for (unsigned int i = 0; i < count; i++) {
  //   if (g_glow[i].m_pEntity == nullptr)
  //     continue;
  //   cout << dec << "Nr: " << i << endl;
  //   cout << hex << "Addr:" << g_glow[i].m_pEntity << endl;
  //   mem.read(g_glow[i].m_pEntity, entity, sizeof(EntityType));
  //   printf("ID: %d\n", entity->m_iEntityId);
  //   cout << "hp: " << entity->m_iHealth << endl;
  //   printf("Origin x=%f y=%f z=%f\n", entity->m_vecOrigin.x,
  //          entity->m_vecOrigin.y, entity->m_vecOrigin.z);
  //   printf("m_fFlags: %u\n", entity->m_fFlags);
  //   printf("m_iEFlags: %d\n", entity->m_iEFlags);
  //   vector<int> diffs = mem.diffMem((addr_type)g_glow[i].m_pEntity, 0x200);
  //   if (diffs.size() > 0) {
  //     // for (int i : diffs)
  //     // cout << hex << i << endl;
  //     mem.printBlock((addr_type)g_glow[i].m_pEntity, 0x200);
  //   }
  //   cout << "-----" << endl;
  // }
}

void GameManager::printPlayerLocationsToFile(const string &filename) {
  // if (players.empty())
  //   return;
  // ofstream file;
  // file.open(filename);
  // // print local player index into array[0,0]
  // file << local_player_index << ",0,0,0,0,0" << endl;
  // // Format: number,hp,team,x,y,z
  // int i = 0;
  // for (PlayerPtr player : players) {
  //   file << i << ",";
  //   file << player->m_iHealth << ",";
  //   file << player->m_iTeamNum << ",";
  //   file << player->m_vecOrigin.x << "," << player->m_vecOrigin.y << ","
  //        << player->m_vecOrigin.z << endl;
  //   i++;
  // }
  // file.close();
}

MemoryAccess &GameManager::getMemoryAccess() { return mem; }

PlayerPtr GameManager::makePlayerPtr(addr_type entity) {
  addr_type entity_health_addr{entity + pattern_scanner.getHealthOffset()};
  addr_type entity_team_addr{entity + pattern_scanner.getTeamNumberOffset()};
  addr_type game_scene_node_addr{entity +
                                 pattern_scanner.getGameSceneNodeOffset()};
  auto const health = mem.read_uint32((void *)entity_health_addr);
  auto const team = static_cast<Team>(mem.read_uint8((void *)entity_team_addr));
  auto const game_scene_node = mem.get_address((void *)game_scene_node_addr);
  auto const abs_origin_addr =
      game_scene_node + pattern_scanner.getAbsOriginOffset();
  bool const is_defusing = isDefusing(entity);
  auto const weapon = getWeapon(entity);
  auto view_angles = getNetworkAngles(entity);
  auto const aim_punch = getAimPunch(entity);
  auto const view_origin = getViewOrigin(entity);
  Vector origin;
  mem.read((void *)abs_origin_addr, &origin, sizeof(origin));

  auto player =
      std::make_shared<Player>(entity, health, origin, team, is_defusing,
                               weapon, view_angles, aim_punch, view_origin);
  return player;
}

addr_type GameManager::getEntityFromHandle(std::uint32_t handle) {
  auto index = handle & 0x7FFF;
  auto chunkIndex = index / CConcreteEntityList::kNumberOfIdentitiesPerChunk;
  CConcreteEntityList entity_list{};
  addr_type entity_list_addr{pattern_scanner.getEntitySystem() +
                             pattern_scanner.getEntityListOffset()};
  mem.read((void *)entity_list_addr, &entity_list, sizeof(CConcreteEntityList));
  auto *chunk = entity_list.chunks[chunkIndex];
  const auto indexInChunk =
      index % CConcreteEntityList::kNumberOfIdentitiesPerChunk;
  CConcreteEntityList::EntityChunk entity_chunk{};
  mem.read(chunk, &entity_chunk, sizeof(entity_chunk));
  CEntityIdentity entityIdentity = entity_chunk[indexInChunk];
  if (entityIdentity.handle == handle) {
    return (addr_type)entityIdentity.entity;
  }
  return 0;
}

PlayerPtr GameManager::getLocalPlayer() {
  addr_type local_player_controller{pattern_scanner.getLocalPlayerController()};
  if (local_player_controller == 0) {
    connected = false;
    throw runtime_error("No local player");
  } else {
    addr_type local_player_pawn_addr{local_player_controller +
                                     pattern_scanner.getPawnHandleOffset()};
    auto player_pawn_handle = mem.read_uint32((void *)local_player_pawn_addr);
    auto entity = getEntityFromHandle(player_pawn_handle);
    local_player = makePlayerPtr(entity);
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

// addr_type GameManager::getPlayerAddr(PlayerPtr player) {
//   for (unsigned int i = 0; i < players.size(); i++) {
//     if (player == players[i])
//       return player_addrs[i];
//   }
//   return 0;
// }

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

Vector GameManager::getViewOrigin(addr_type player_addr) {
  Vector view_origin;
  if (!mem.read((void *)(player_addr + 4908), &view_origin,
                sizeof(view_origin)))
    throw runtime_error("Could not get view offset.");
  return view_origin;
}

QAngle GameManager::getAimPunch(addr_type player_addr) {
  QAngle ang;
  // auto diffs = mem.diffMem(player_addr, 10000);
  // for (auto i : diffs) {
  //   QAngle angle;
  //   mem.read(player_addr + i, &angle, sizeof(angle));
  //   printVec(std::to_string(i), angle);
  // }

  if (!mem.read((void *)(player_addr + 5420), &ang, sizeof(ang)))
    throw runtime_error("Could not get AimPunch.");
  return ang;
}

bool GameManager::isDefusing(addr_type player_addr) {
  char buf;
  if (!mem.read(player_addr + pattern_scanner.getIsDefusingOffset(), &buf,
                sizeof(buf)))
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
  // auto diffs = mem.diffMem(player_addr, 8192);
  // for (auto i : diffs) {
  //   QAngle angle;
  //   mem.read(player_addr + i, &angle, sizeof(angle));
  //   printVec(std::to_string(i), angle);
  // }
  // Offset found by running diffMem and sanity checking
  QAngle ang;
  if (!mem.read((void *)(player_addr + 0x1338), &ang, sizeof(ang)))
    throw runtime_error("Could not get NetworkAngles.");
  return ang;
}

Weapon GameManager::getWeapon(addr_type player_addr) {
  // cout << "Reading weapon_service_addr" << endl;
  auto const weapon_service_addr = mem.get_address(
      (void *)(player_addr + pattern_scanner.getWeaponServicesOffset()));
  // cout << "Reading weapon_handle" << endl;
  auto const weapon_handle = mem.read_uint32(
      (void *)(weapon_service_addr + pattern_scanner.getActiveWeaponOffset()));
  auto const active_weapon_addr = getEntityFromHandle(weapon_handle);
  // cout << "active_weapon_addr: " << active_weapon_addr << endl;
  auto weapon_id =
      mem.read_uint32((void *)(active_weapon_addr + 0x1140 + 0x50 + 0x1BA));
  // cout << "weapon_id:" << weapon_id << endl;
  weapon_id &= 0xFFF;
  return (Weapon)weapon_id;
}

std::vector<Vector> GameManager::getSmokeLocations() {
  // super hacky way to detect smokes from the entity list:
  // if a non player entity has FL_ONGROUND set it's a smoke
  std::vector<Vector> result;
  for (auto entity : nonPlayerEntities) {
    // if (!(entity->m_fFlags & FL_CLIENT) && (entity->m_fFlags & FL_ONGROUND))
    // {
    //   result.push_back(entity->m_vecOrigin);
    // }
  }
  return result;
}

bool GameManager::lineThroughSmoke(Vector start, Vector end) {
  std::vector<Vector> smokeLocations = getSmokeLocations();
  Vector d = end - start;
  normalize_vector(d);
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
