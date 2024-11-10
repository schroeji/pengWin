#include "ValveBSP/BSPParser.hpp"
#include "misc/clicker.hpp"
#include "misc/manager.hpp"
#include "misc/memory_access.hpp"
#include "misc/settings.hpp"
#include "misc/typedef.hpp"

class Aimer {

private:
  GameManager &csgo;
  MemoryAccess &mem;
  Clicker clicker;
  int uinput;
  Settings &settings;
  const float angle_multiplier = 1 / 0.0220031738281250;
  const float inverse_sens;
  BSPParser &bspParser;
  const float angle_multiplier_scoped = 1 / 0.0127111077308654;
  Vector getView(bool);
  Vector predictPositionOffset(PlayerPtr);
  // dispatches to the correct target finding function based on weaponType and
  // other factors
  std::pair<PlayerPtr, Vector> findTargetDispatcher(Vector, unsigned int);
  // default target finding function good for most weapons
  std::pair<PlayerPtr, Vector> closestTargetInFov(Vector, float);
  std::pair<PlayerPtr, Vector> zeusTarget(Vector, float);
  // dispatches to the correct calcMouseMovement function based on weaponType
  // and other factors
  MouseMovement mouseMovementDispatcher(QAngle, Vector, bool, unsigned int i);
  // default calcMouseMovement good for most weapons
  MouseMovement default_calcMouseMovement(QAngle, Vector, bool);
  MouseMovement spline_calcMouseMovement(QAngle, Vector, unsigned int);
  // store variables for spline_calcMouseMovement so they don't have to be
  // recalculated in every call
  float spline_a[2] = {0, 0};
  float spline_b[2] = {0, 0};
  float spline_x_len;
  float spline_last_x;
  QAngle spline_start_angle;
  QAngle spline_supp_angle;
  void createFakeInputDevice();

public:
  Aimer(GameManager &csgo, BSPParser &);
  ~Aimer();
  // Routine to call by thread; unsigned int i is the number of the call
  void aimCheck(unsigned int);
  // move the mouse by MouseMovement
  void moveAim(MouseMovement);
};
