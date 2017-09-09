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
  const float angle_multiplier = 1 / 0.0220031738281250;
  const float sensitivity = 1.3;
  const float inverse_sens = 1 / sensitivity;
  const float angle_multiplier_scoped = 1 / 0.0127111077308654;
  Vector getView();
  Vector predictPositionOffset(EntityType*);
  EntityType* closestTargetInFov();

public:
  Aimer(GameManager& csgo);
  ~Aimer();
  MouseMovement calcMouseMovement(Vector, Vector);
  void moveAim(MouseMovement);
  void aimCheck();
};
