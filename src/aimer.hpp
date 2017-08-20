#include "typedef.hpp"
#include "manager.hpp"
#include "memory_access.hpp"
#include "clicker.hpp"

class Aimer {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Clicker clicker;
  Display* display;
  Window root_window;

public:
  Aimer(GameManager& csgo);
  void setAim(EntityType*);
  void xSetAim(EntityType*);
  void moveAim(int, int);
};
