#include "typedef.hpp"
#include "memory_access.hpp"
#include "manager.hpp"

class Trigger {
private:
  GameManager& csgo;
  MemoryAccess& mem;

public:
  Trigger(GameManager& csgo);
  void triggerCheck();
};
