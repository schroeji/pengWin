#include "memory_access.hpp"
#include "manager.hpp"
#include "clicker.hpp"
#include "typedef.hpp"

class Trigger {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Clicker clicker;

public:
  Trigger(GameManager& csgo);
  void triggerCheck();
};
