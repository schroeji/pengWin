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
  std::pair<EntityType*, Vector> findTargetDispatcher(Vector);
  std::pair<EntityType*, Vector> closestTargetInFov(Vector, float);
  std::pair<EntityType*, Vector> zeusTarget(Vector, float);
  MouseMovement mouseMovementDispatcher(QAngle, Vector, bool, unsigned int i);
  MouseMovement default_calcMouseMovement(QAngle, Vector, bool);
  MouseMovement spline_calcMouseMovement(QAngle, Vector, unsigned int);
  float spline_a[2] = {0, 0};
  float spline_b[2] = {0, 0};
  float spline_x_len;
  float spline_last_x;
  QAngle spline_start_angle;
  QAngle spline_supp_angle;
public:
  Aimer(GameManager& csgo);
  ~Aimer();
  void aimCheck(unsigned int);
  void moveAim(MouseMovement);
};
