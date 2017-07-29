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

private:
  MemoryAccess& mem;
  ObjectManager manager;
  std::vector<EntityType*> players;
};
