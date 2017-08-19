#include "manager.hpp"
#include "memory_access.hpp"
#include "typedef.hpp"

class BunnyHopper {
private:
  MemoryAccess& mem;
  void jump();

public:
  BunnyHopper(GameManager csgo);
  void jumpCheck();
};
