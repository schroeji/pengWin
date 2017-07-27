#include "typedef.hpp"
#include "memory_access.hpp"

class Trigger {
private:
  MemoryAccess& mem;
  GameManager& csgo;

public:
  Trigger(GameManager& csgo);
  void triggerCheck();
}
