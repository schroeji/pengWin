#include "typedef.hpp"
#include "manager.hpp"
#include "memory_access.hpp"
#include "clicker.hpp"

class Aimer {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Clicker clicker;
public:
  Aimer(GameManager& csgo);
  void setAim(EntityType*);
};
