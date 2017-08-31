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
  bool gameRunning();
  bool isOnServer();

private:
  MemoryAccess& mem;
  ObjectManager manager;
  std::vector<EntityType*> players;
  // number of the local_player in players
  int local_player_index = -1;
};
