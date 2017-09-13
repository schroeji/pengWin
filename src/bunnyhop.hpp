#include "manager.hpp"
#include "memory_access.hpp"
#include "typedef.hpp"
#include "settings.hpp"

class BunnyHopper {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Settings settings;
  void jump();

public:
  BunnyHopper(GameManager& csgo);
  ~BunnyHopper();
  void jumpCheck(unsigned int);
};
