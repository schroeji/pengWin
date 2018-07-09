#pragma once
#include "memory_access.hpp"
#include "typedef.hpp"
#include "util.hpp"

#include <string>
#include <vector>

class GameManager {
public:
  GameManager(MemoryAccess& mem);
  void grabPlayers();
  void printPlayers();
  void printEntities();
  void printPlayerLocationsToFile(const std::string&);
  MemoryAccess& getMemoryAccess();
  std::vector<EntityType*>& getPlayers();
  EntityType* getLocalPlayer();
  int getLocalPlayerIndex();
  bool gameRunning();
  bool isOnServer();
  addr_type getPlayerAddr(EntityType*);
  std::string getMapName();
  bool isScoped(addr_type);
  bool isDefusing(addr_type);
  Weapon getWeapon(addr_type);
  Team getTeam(addr_type);
  unsigned int getCrosshairTarget(addr_type);
  QAngle getAimPunch(addr_type);
  std::vector<Vector> getSmokeLocations();
  bool lineThroughSmoke(Vector start, Vector end);

private:
  MemoryAccess& mem;
  std::vector<EntityType*> players;
  std::vector<EntityType*> nonPlayerEntities;
  std::vector<addr_type> player_addrs;
  // number of the local_player in players
  int local_player_index = -1;
  EntityType* local_player = new EntityType;
  bool connected = false;
  GlowObjectManager_t manager;
  GlowObjectDefinition_t g_glow[1024];
};
