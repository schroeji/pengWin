#pragma once
#include "memory_access.hpp"
#include "typedef.hpp"

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
  Team getTeam();
  unsigned int getCrosshairTarget();
  QAngle getAimPunch();

private:
  MemoryAccess& mem;
  ObjectManager manager;
  std::vector<EntityType*> players;
  std::vector<addr_type> player_addrs;
  // number of the local_player in players
  int local_player_index = -1;
  EntityType* local_player = new EntityType;
};
