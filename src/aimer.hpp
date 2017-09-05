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
  const float angle_multiplier_y = 1 / 0.0220031738281250;
  const float angle_multiplier_x = 1 / 0.0220031738281250;
  const float sensitivity = 1.3;
  const float inverse_sens = 1 / sensitivity;
  Vector getView();
  EntityType* closestTargetInFov();

public:
  Aimer(GameManager& csgo);
  ~Aimer();
  void setAim(EntityType*);
  void xSetAim(Vector);
  void moveAim(int, int);
  void aimLoop();
};
