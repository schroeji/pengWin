#include "manager.hpp"
#include "memory_access.hpp"
#include "netvar_finder.hpp"
#include "typedef.hpp"
#include "util.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

using namespace std;

GameManager::GameManager(MemoryAccess &mem)
    : mem(mem), netvar_finder(mem), pattern_scanner(mem),
      settings(Settings::getInstance()) {
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
  netvar_finder.dump();
  netvar_finder.printNetvars();
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
    cout << "Weapon: " << getWeaponName(player->weapon) << endl;
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
}

MemoryAccess &GameManager::getMemoryAccess() { return mem; }

std::array<BoneInfo, MAX_BONES> GameManager::getBoneMatrix(addr_type player) {
  if (player == 0)
    throw runtime_error("getBoneMatrix: Player is nullptr.");
  addr_type const game_scene_node_addr{player +
                                 pattern_scanner.getGameSceneNodeOffset()};
  auto const game_scene_node = mem.get_address((void *)game_scene_node_addr);
  addr_type const bone_matrix_ptr {game_scene_node + netvar_finder.getNetvar("m_modelState") + 0x80};
  addr_type bone_matrix_addr = mem.get_address((void*)bone_matrix_ptr);
  // mem.printBlock(bone_matrix_addr, 0x100);
  std::array<BoneInfo, MAX_BONES> boneMatrix;
  if (!mem.read(bone_matrix_addr, boneMatrix.data(), boneMatrix.size() * sizeof(BoneInfo)))
    throw runtime_error("Could not read boneMatrix.");
  return boneMatrix;
}

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

string GameManager::getMapName() {
  if (current_map_ == "") {
    if (settings.find_map) {
      cout << "Scanning for map..." << endl;
      if (settings.radar_generic)
        current_map_ = "generic";
      auto global_vars = pattern_scanner.getGlobalVars();
      if(settings.debug)
        cout << "Global vars: " << global_vars << endl;
      for (int i = 0; i < 0x300; i += 8) {
        addr_type addr = mem.read_uint64((void *)(global_vars + i));
        auto map_name_candidate = mem.read_string((void *)addr);
        if(map_name_candidate.substr(0,5) == "maps/") {
          auto map_file_name = map_name_candidate.substr(5);
          current_map_ = split_string(map_file_name, ".")[0];
          cout << "Found Map: " << current_map_ << endl;
          break;
        }
      }
      if (current_map_ == "") {
        cout << "Could not find map. Please choose map:" << endl;
        cin >> current_map_;
      }
      this_thread::sleep_for(chrono::milliseconds(1000));
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

  if (!mem.read(
          (void *)(player_addr + netvar_finder.getNetvar("m_aimPunchAngle")),
          &ang, sizeof(ang)))
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
  if (!mem.read((void *)(player_addr + netvar_finder.getNetvar("v_angle")),
                &ang, sizeof(ang)))
    throw runtime_error("Could not get NetworkAngles.");
  return ang;
}

Weapon GameManager::getWeapon(addr_type player_addr) {
  auto const m_pClippingWeapon = mem.get_address(
      (void *)(player_addr + netvar_finder.getNetvar("m_pClippingWeapon")));
  auto const weapon_handle =
      mem.get_address((void *)(m_pClippingWeapon + 0x10));
  if (weapon_handle == 0) {
    return Weapon::KNIFE;
  }
  auto const weapon_name_addr = mem.get_address((void *)(weapon_handle + 0x20));
  auto name = mem.read_string((void *)weapon_name_addr);
  return getWeaponByName(name);
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
