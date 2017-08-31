#include "typedef.hpp"
#include "manager.hpp"
#include "memory_access.hpp"
#include "clicker.hpp"
#include "settings.hpp"

class Aimer {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Clicker clicker;
  int uinput;
  Settings settings;

public:
  Aimer(GameManager& csgo);
  ~Aimer();
  void setAim(EntityType*);
  void xSetAim(EntityType*);
  void moveAim(int, int);
};
