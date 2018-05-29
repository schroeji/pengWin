#include "misc/typedef.hpp"
#include "misc/util.hpp"
#include "aimer.hpp"
#include "misc/manager.hpp"
#include "misc/clicker.hpp"
#include "misc/settings.hpp"

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
#include <stdexcept>

using namespace std;
Aimer::Aimer(GameManager& csgo) : csgo(csgo),
                                  mem(csgo.getMemoryAccess()),
                                  clicker(Clicker(csgo.getMemoryAccess())),
                                  settings(Settings::getInstance()),
                                  inverse_sens(1 / settings.sensitivity) {
  // prepare mouse
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

void Aimer::aimCheck(unsigned int i) {
  EntityType* local_player;
  EntityType* enemy;
  Vector target_pos;
  Vector view;
  try {
    local_player = csgo.getLocalPlayer();
    // don't recoil compensate for tap shooting
    view = getView(i > 200/settings.aim_sleep);
    pair<EntityType*, Vector> temp = closestTargetInFov(view);
    enemy = temp.first;
    target_pos = temp.second;
  } catch(const runtime_error& e) {
    if (settings.debug) cout << "EXCEPTION:" << e.what() << endl;
    this_thread::sleep_for(chrono::milliseconds(settings.aim_sleep));
    return;
  }
  Vector player_pos = {local_player->m_vecOrigin.x,
                       local_player->m_vecOrigin.y + local_player->m_vecViewOffset.y,
                       local_player->m_vecOrigin.z};
  Vector dist = getDist(&player_pos, &target_pos);

  dist = dist + predictPositionOffset(enemy);
  if (settings.debug) printf("distance vector: %f, %f, %f\n", dist.x, dist.y, dist.z);
  if (settings.debug) printf("player_pos %f, %f, %f\n", player_pos.x, player_pos.y, player_pos.z);
  if (settings.debug) printf("target_pos %f, %f, %f\n", target_pos.x, target_pos.y, target_pos.z);
  if (dist.x == 0 && dist.y == 0 && dist.z == 0)
    return;
  bool use_smooth = settings.aim_smooth_first_shot || i != 0;
  MouseMovement move = calcMouseMovement(view, dist, use_smooth);

  if (settings.debug) cout << dec << "move angle x: " << move.x << endl;
  if (settings.debug) cout << dec << "move angle y: " << move.y << endl;
  if (move.x == 0 && move.y == 0) {
    this_thread::sleep_for(chrono::milliseconds(settings.aim_sleep));
    return;
  }
  try {
    moveAim(move);
    if (settings.aim_autoshoot && len(move) < 4)
      clicker.xClick();
  } catch(const runtime_error& e) {
    if (settings.debug) cout << "EXCEPTION:" << e.what() << endl;
  }
  this_thread::sleep_for(chrono::milliseconds(settings.aim_sleep));
}

MouseMovement Aimer::calcMouseMovement(Vector view, Vector dist, bool use_smooth) {
  normalize_vector(&view);
  normalize_vector(&dist);
  if (settings.debug) printf("view: %f, %f, %f\n", view.x, view.y, view.z);
  Vector2D view_x_z_projection = {view.x, view.z};
  Vector2D view_y_z_projection = {view.y, view.z};
  Vector2D dist_x_z_projection = {dist.x, dist.z};
  Vector2D dist_y_z_projection = {dist.y, dist.z};
  normalize_vector(&view_x_z_projection);
  normalize_vector(&view_y_z_projection);
  normalize_vector(&dist_x_z_projection);
  normalize_vector(&dist_y_z_projection);

  // needed for calculating the y angle
  Vector tmp = {dist.x, view.y, dist.z};
  normalize_vector(&tmp);

  float missing_angle_x = view_x_z_projection * dist_x_z_projection;
  float missing_angle_y = tmp * dist;
  // restrict for acos
  missing_angle_x = min(1.f, max(-1.f , missing_angle_x));
  missing_angle_y = min(1.f, max(-1.f , missing_angle_y));
  missing_angle_x = acos(missing_angle_x);
  missing_angle_y = acos(missing_angle_y);
  // determinante gives the orientation of the two vectors
  float det_x = view.x * dist.z - view.z * dist.x;
  float orientation_x = sgn(det_x);
  // because both are normalized the one with bigger y component
  // is counter-clockwise of the other in the 2D plane
  float orientation_y = sgn(tmp.y - dist.y);
  missing_angle_x = radian_to_degree(missing_angle_x);
  missing_angle_y = radian_to_degree(missing_angle_y);
  float multiplier = angle_multiplier;
  if (csgo.isScoped(mem.local_player_addr))
    multiplier = angle_multiplier_scoped;
  float smooth = use_smooth ? settings.smoothing_factor : 1.0;
  int moveAngle_x = static_cast<int>(orientation_x * missing_angle_x * multiplier * inverse_sens * smooth);
  int moveAngle_y = static_cast<int>(orientation_y * missing_angle_y * multiplier * inverse_sens * smooth);
  return {moveAngle_x, moveAngle_y};
}

void Aimer::moveAim(MouseMovement move) {
  int dx = move.x;
  int dy = move.y;
  struct input_event ev;
  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_REL;
  ev.code = REL_X;
  ev.value = dx;
  if(write(uinput, &ev, sizeof(struct input_event)) < 0)
    throw runtime_error("Could not write mouse movement.");
  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_REL;
  ev.code = REL_Y;
  ev.value = dy;
  if(write(uinput, &ev, sizeof(struct input_event)) < 0)
    throw runtime_error("Could not write mouse movement.");

  memset(&ev, 0, sizeof(struct input_event));
  ev.type = EV_SYN;
  if(write(uinput, &ev, sizeof(struct input_event)) < 0)
    throw runtime_error("Could not write mouse movement.");
}

Vector Aimer::getView(bool rcs) {
  EntityType* local_player;
  try {
    local_player = csgo.getLocalPlayer();
  } catch (const runtime_error& e) {
    throw e;
  }
  QAngle currAngle = local_player->m_angNetworkAngles;
  // times two because of weapon_recoil_scale convar
  QAngle aimPunch = csgo.getAimPunch(mem.local_player_addr) * 2.0;
  if (rcs)
    currAngle = currAngle + aimPunch;
  float radians_x = degree_to_radian(-currAngle.x); // pitch
  float radians_y = degree_to_radian(currAngle.y);  // yaw
  float v1 = cos(radians_x) * sin(radians_y);
  float v2 = sin(radians_x);
  float v3 = cos(radians_x) * cos(radians_y);
  // is normalized because sin*sin + cos*cos = 1
  return {v1, v2, v3};
}

pair<EntityType*, Vector> Aimer::closestTargetInFov(Vector view) {
  EntityType* local_player;
  Vector bone_pos;
  try {
    local_player = csgo.getLocalPlayer();
  } catch (const runtime_error& e) {
    throw e;
  }
  Vector player_pos = {local_player->m_vecOrigin.x,
                       local_player->m_vecOrigin.y + local_player->m_vecViewOffset.y,
                       local_player->m_vecOrigin.z};
  vector<EntityType*> players = csgo.getPlayers();
  unsigned int boneIds[] = {3, 6, 7, 8, 66, 67, 73, 74};
  if (players.size() < 2)
    throw runtime_error("Only one player.");
  EntityType* closestPlayer = nullptr;
  float closestAngle = settings.aim_fov;
  Vector closestBone = {0, 0, 0};
  Team team = csgo.getTeam(mem.local_player_addr);
  for (EntityType* enemy : players) {
    if (enemy == local_player || enemy->m_iTeamNum == team)
      continue;
    if (settings.smoke_check && csgo.lineThroughSmoke(player_pos, enemy->m_vecOrigin))
      continue;
    addr_type enemy_addr = csgo.getPlayerAddr(enemy);
    for (unsigned int boneID : boneIds ) {
      // cout << "bone ID:" << boneID << endl;
      try {
        bone_pos = mem.getBone(enemy_addr, boneID);
        // printf("bone: %f, %f, %f \n", bone_pos.x, bone_pos.z, bone_pos.z);
      } catch(const runtime_error& e) {
        if (settings.debug) cout << e.what() << endl;;
      }
      // find angle between player and target
      Vector dist = getDist(&player_pos, &bone_pos);
      normalize_vector(&dist);
      float angle = acos(dist * view);
      if (angle > (settings.aim_fov / 2.))
        continue;
      if (closestPlayer == nullptr) {
        closestPlayer = enemy;
        closestAngle = angle;
        closestBone = bone_pos;
      } else if (closestAngle > angle) {
        closestPlayer = enemy;
        closestAngle = angle;
        closestBone = bone_pos;
      }
    }
  }
  if (closestPlayer == nullptr)
    throw runtime_error("No player in FOV");
  return pair<EntityType*, Vector>(closestPlayer, closestBone);
}

Vector Aimer::predictPositionOffset(EntityType* player) {
  Vector vel = {player->m_vecVelocity.y, player->m_vecVelocity.z, player->m_vecVelocity.x};
  float t = (float) settings.main_loop_sleep / 1000.0;
  return vel * t;
}
