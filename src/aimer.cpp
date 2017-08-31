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
  // EntityType* local_player = csgo.getLocalPlayer();
  EntityType* local_player = csgo.getPlayers()[0];
  Vector player_pos = local_player->m_vecOrigin;
  Vector enemy_pos = enemy->m_vecOrigin;
  Vector dist = getDist(&player_pos, &enemy_pos);
  printf("dist: %f, %f, %f\n", dist.x, dist.y, dist.z);
  normalize_vector(&dist);
  printf("normalized dist: %f, %f, %f\n", dist.x, dist.y, dist.z);
  QAngle currAngle = local_player->m_angNetworkAngles;
  float radians = degree_to_radian(currAngle.y);
  float y = cos(radians);
  // Vector current_view = {0, y, sqrt(1 - y*y)};
  // Vector current_view = {0, sqrt(1 - y*y), y};
  float x = radians > 0 ? sqrt(1 - y*y) : -sqrt(1 - y*y);
  Vector current_view = {x, 0, y};
  printf("view: %f, %f, %f\n", current_view.x, current_view.y, current_view.z);
  float missing_angle = acos(scalar_prod(&dist, &current_view));
  float orientation = 0;
  float det = current_view.x * dist.z - current_view.z * dist.x;
  if (det > 0) {
    orientation = 1;
  } else if (det < 0) {
    orientation = -1;
  } else {
    orientation = 0;
  }
  missing_angle = radian_to_degree(missing_angle);
  int moveAngle = static_cast<int>(orientation * (missing_angle*10));
  moveAim(moveAngle, 0);
  cout << "missing angle: " << missing_angle << endl;
  cout << dec << "move angle: " << moveAngle << endl;
  cout << "orient:" << orientation << endl;
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
