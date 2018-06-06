#include "misc/typedef.hpp"
#include "misc/manager.hpp"
#include "misc/memory_access.hpp"
#include "misc/clicker.hpp"
#include "misc/settings.hpp"

class Aimer {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Clicker clicker;
  int uinput;
  Settings& settings;
  const float angle_multiplier = 1 / 0.0220031738281250;
  const float inverse_sens;
  const float angle_multiplier_scoped = 1 / 0.0127111077308654;
  Vector getView(bool);
  Vector predictPositionOffset(EntityType*);
  std::pair<EntityType*, Vector> closestTargetInFov(Vector view);
  MouseMovement calcMouseMovement(QAngle, Vector, bool);

public:
  Aimer(GameManager& csgo);
  ~Aimer();
  void aimCheck(unsigned int);
  void moveAim(MouseMovement);
};
