#include "misc/memory_access.hpp"
#include "misc/manager.hpp"
#include "misc/clicker.hpp"
#include "misc/typedef.hpp"
#include "misc/settings.hpp"

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
