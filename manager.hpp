#include "memory_access.hpp"
#include "typedef.hpp"

#include <string>
#include <vector>
class GameManager{
public:
  GameManager(MemoryAccess& mem);
  void getPlayers();
  void printPlayers();
  void printPlayerLocationsToFile(const std::string&);

private:
  MemoryAccess& mem;
  ObjectManager manager;
  std::vector<EntityType*> players;
};
