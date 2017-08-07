#include "typedef.hpp"
#include "aimer.hpp"
#include "manager.hpp"
#include "clicker.hpp"


#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>
#include <math.h>

using namespace std;
Aimer::Aimer(GameManager& csgo) : csgo(csgo),
                                  mem(csgo.getMemoryAccess()),
                                  clicker(Clicker(csgo.getMemoryAccess())){

  display= XOpenDisplay(NULL);
  root_window = XRootWindow(display, 0);
}

void Aimer::setAim(EntityType* enemy) {
}

// needs m_rawinput 0
void Aimer::xSetAim(EntityType* enemy) {
  if (enemy == nullptr) {
    XSelectInput(display, root_window, KeyReleaseMask);
    XWarpPointer(display, None, root_window, 0, 0, 0, 0, 959, 540);
    XFlush(display);
    return;
  }
  EntityType* local_player = csgo.getLocalPlayer();
  Vector player_pos = local_player->m_vecOrigin;
  Vector enemy_pos = enemy->m_vecOrigin;
  Vector dist = getDist(&player_pos, &enemy_pos);
  printf("dist: %f, %f, %f\n", dist.x, dist.y, dist.z);
  normalize_vector(&dist);
  QAngle currAngle = local_player->m_angRotation;
  float radians = degree_to_radian(currAngle.y);
  float y = cos(radians);
  Vector current_view = {sqrt(1 - y*y), y, 0};
  printf("view: %f, %f, %f\n", current_view.x, current_view.y, current_view.z);
  float missing_angle = acos(scalar_prod(&dist, &current_view));
  cout << "missing angle: " << missing_angle << endl;
}
