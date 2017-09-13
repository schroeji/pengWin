#include "memory_access.hpp"
#include "manager.hpp"
#include "clicker.hpp"
#include "typedef.hpp"
#include "settings.hpp"

class Trigger {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Clicker clicker;
  Settings settings;

public:
  Trigger(GameManager& csgo);
  ~Trigger();
  void triggerCheck(unsigned int);
};
