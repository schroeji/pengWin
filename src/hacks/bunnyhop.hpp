#include "misc/manager.hpp"
#include "misc/memory_access.hpp"
#include "misc/typedef.hpp"
#include "misc/settings.hpp"

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
