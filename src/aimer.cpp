#include "typedef.hpp"
#include "aimer.hpp"
#include "manager.hpp"
#include "clicker.hpp"
#include "settings.hpp"


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <chrono>
#include <thread>

using namespace std;
Aimer::Aimer(GameManager& csgo) : csgo(csgo),
                                  mem(csgo.getMemoryAccess()),
                                  clicker(Clicker(csgo.getMemoryAccess())),
                                  settings(Settings::getInstance()) {
  struct uinput_user_dev uidev;
  uinput = open("/dev/input/uinput", O_WRONLY | O_NONBLOCK);

  if(uinput < 0)
    uinput = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if(uinput < 0) {
    cout << "WARNING: could not open uinput" << endl;
    return;
  }

  if(ioctl(uinput, UI_SET_EVBIT, EV_KEY) < 0) {
    cout << "Error while setting up aimbot" << endl;
    return;
  }
  if(ioctl(uinput, UI_SET_KEYBIT, BTN_LEFT) < 0) {
    cout << "Error while setting up aimbot" << endl;
    return;
  }

  if(ioctl(uinput, UI_SET_EVBIT, EV_REL) < 0) {
    cout << "Error while setting up aimbot" << endl;
    return;
  }

  if(ioctl(uinput, UI_SET_RELBIT, REL_X) < 0) {
    cout << "Error while setting up aimbot" << endl;
    return;
  }

  if(ioctl(uinput, UI_SET_RELBIT, REL_Y) < 0) {
    cout << "Error while setting up aimbot" << endl;
    return;
  }

  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "USB-Mouse");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1;
  uidev.id.product = 0x1;
  uidev.id.version = 1;

  if(write(uinput, &uidev, sizeof(uidev)) < 0) {
    cout << "Error while creating fake input device" << endl;
    return;
  }

  if(ioctl(uinput, UI_DEV_CREATE) < 0) {
    cout << "Error while creating fake input device" << endl;
    return;
  }
}

Aimer::~Aimer() {
  if(ioctl(uinput, UI_DEV_DESTROY) < 0) {
    cout << "WARNING: Could not destroy fake input device" << endl;
    return;
  }
  close(uinput);
}

void Aimer::setAim(EntityType* enemy) {
}

// needs m_rawinput 0
void Aimer::xSetAim(EntityType* enemy) {
  if (enemy == nullptr)
    return;
  EntityType* local_player = csgo.getLocalPlayer();
  Vector player_pos = local_player->m_vecOrigin;
  Vector enemy_pos = enemy->m_vecOrigin;
  Vector dist = getDist(&player_pos, &enemy_pos);
  if (dist.x == 0 && dist.y ==0 && dist.z ==0)
    return;

  printf("dist: %f, %f, %f\n", dist.x, dist.y, dist.z);
  normalize_vector(&dist);
  printf("normalized dist: %f, %f, %f\n", dist.x, dist.y, dist.z);
  QAngle currAngle = local_player->m_angNetworkAngles;
  printf("curr angle: %f, %f, %f\n", currAngle.x, currAngle.y, currAngle.z);
  float radians_x = degree_to_radian(-currAngle.x); // pitch
  float radians_y = degree_to_radian(currAngle.y);  // yaw
  float v1 = cos(radians_x) * sin(radians_y);
  float v2 = sin(radians_x);
  float v3 = cos(radians_x) * cos(radians_y);
  Vector current_view = {v1, v2, v3};
  printf("view: %f, %f, %f\n", current_view.x, current_view.y, current_view.z);
  Vector2D view_x_z_projection = {v1, v3};
  Vector2D view_y_z_projection = {v2, v3};
  Vector2D dist_x_z_projection = {dist.x, dist.z};
  Vector2D dist_y_z_projection = {dist.y, dist.z};
  normalize_vector(&view_x_z_projection);
  normalize_vector(&view_y_z_projection);
  normalize_vector(&dist_x_z_projection);
  normalize_vector(&dist_y_z_projection);

  float missing_angle_x = view_x_z_projection * dist_x_z_projection;
  // missing_angle_x /= len(view_x_z_projection) * len(dist_x_z_projection);
  // float missing_angle_y = view_y_z_projection * dist_y_z_projection;
  Vector tmp = {dist.x, current_view.y, dist.z};
  normalize_vector(&tmp);
  printf("tmp: %f, %f, %f\n", tmp.x, tmp.y, tmp.z);
  float missing_angle_y = tmp * dist;
  // missing_angle_y /= len(view_y_z_projection) * len(dist_y_z_projection);
  missing_angle_x = acos(missing_angle_x);
  missing_angle_y = acos(missing_angle_y);
  missing_angle_x = radian_to_degree(missing_angle_x);
  missing_angle_y = radian_to_degree(missing_angle_y);
  // determinante gives the orientation of the two vectors
  float det_x = current_view.x * dist.z - current_view.z * dist.x;
  float orientation_x = 0;
  if (det_x > 0) {
    orientation_x = 1;
  } else if (det_x < 0) {
    orientation_x = -1;
  } else {
    orientation_x = 0;
  }
  float orientation_y = 0;
  if (tmp.y > dist.y) {
    orientation_y = 1;
  } else if (tmp.y < dist.y) {
    orientation_y = -1;
  } else {
    orientation_y = 0;
  }
  int moveAngle_x = static_cast<int>(orientation_x * missing_angle_x * angle_multiplier_x * inverse_sens);
  int moveAngle_y = static_cast<int>(orientation_y * missing_angle_y * angle_multiplier_y * inverse_sens);
  moveAim(moveAngle_x, moveAngle_y);
  cout << "missing angle x: " << missing_angle_x << endl;
  cout << "missing angle y: " << missing_angle_y << endl;
  cout << dec << "move angle x: " << moveAngle_x << endl;
  cout << dec << "move angle y: " << moveAngle_y << endl;
  cout << "orient_x:" << orientation_x << endl;
  cout << "orient_y:" << orientation_y << endl;
}

void Aimer::moveAim(int dx, int dy) {
  struct input_event ev;
  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_REL;
  ev.code = REL_X;
  ev.value = dx;
  if(write(uinput, &ev, sizeof(struct input_event)) < 0) {
    cout << "ERROR: could not write mouse movement" << endl;
    return;
  }
  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_REL;
  ev.code = REL_Y;
  ev.value = dy;
  if(write(uinput, &ev, sizeof(struct input_event)) < 0){
    cout << "ERROR: could not write mouse movement" << endl;
    return;
  }

  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_SYN;
  if(write(uinput, &ev, sizeof(struct input_event)) < 0) {
    cout << "ERROR: could not write mouse movement" << endl;
    return;
  }
}
