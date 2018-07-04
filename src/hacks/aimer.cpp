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
#include <cassert>

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
    if(settings.debug) cout << "--- Aim Check ---" << endl;
    local_player = csgo.getLocalPlayer();
    // don't recoil compensate for tap shooting
    view = getView(i > 200/settings.aim_sleep);
    pair<EntityType*, Vector> temp = findTargetDispatcher(view);
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
  QAngle aimPunch = csgo.getAimPunch(mem.local_player_addr) * 2.0;
  MouseMovement move = mouseMovementDispatcher(local_player->m_angNetworkAngles + aimPunch, dist, use_smooth);
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

MouseMovement Aimer::mouseMovementDispatcher(QAngle curr_angle, Vector dist, bool use_smooth) {
  Weapon weapon = csgo.getWeapon(mem.local_player_addr);
  switch(weapon) {
  // pistols
  case Weapon::DEAGLE:
  case Weapon::GLOCK:
  case Weapon::USP_SILENCER:
  case Weapon::FIVESEVEN:
  case Weapon::HKP2000:
  case Weapon::P250:
  case Weapon::TEC9:
    return default_calcMouseMovement(curr_angle, dist, use_smooth);
  case Weapon::REVOLVER:
    return default_calcMouseMovement(curr_angle, dist, use_smooth);
    // snipers
  case Weapon::AWP:
  case Weapon::SSG08:
    return default_calcMouseMovement(curr_angle, dist, false);
    // auto snipers
  case Weapon::G3SG1:
  case Weapon::SCAR20:
    return default_calcMouseMovement(curr_angle, dist, false);
    // disable for nades / knives
  case Weapon::SMOKEGRENADE:
  case Weapon::FLASHBANG:
  case Weapon::HEGRENADE:
  case Weapon::MOLOTOV:
  case Weapon::INCGRENADE:
  case Weapon::DECOY:
  case Weapon::C4:
  case Weapon::KNIFE:
  case Weapon::KNIFE_T:
    return {0, 0};
  default:
    return default_calcMouseMovement(curr_angle, dist, use_smooth);
  }
}

MouseMovement Aimer::default_calcMouseMovement(QAngle curr_angle, Vector dist, bool use_smooth) {
  normalize_vector(&dist);
  if (settings.debug) printf("dist: %f, %f, %f\n", dist.x, dist.y, dist.z);

  // curr_angle and target angle are in format {pitch, yaw, 0}
  QAngle target_angle = {asin(-dist.y), atan2(dist.x, dist.z), 0};
  target_angle = radian_to_degree(target_angle);
  if (settings.debug) printf("curr_angle: %f, %f\n", curr_angle.x, curr_angle.y);
  if (settings.debug) printf("target_angle: %f, %f\n", target_angle.x, target_angle.y);
  QAngle missing_angle = target_angle - curr_angle;
  if (missing_angle.y > 180)
    missing_angle.y -= 360;
  else if (missing_angle.y < -180)
    missing_angle.y += 360;
  if (settings.debug) printf("missing angles: x:%f y:%f\n", missing_angle.x, missing_angle.y);
  cout << settings.aim_fov << endl;
  cout << degree_to_radian(missing_angle.x) << endl;
  cout << degree_to_radian(missing_angle.y) << endl;

  assert(fabs(degree_to_radian(missing_angle.x)) < settings.aim_fov);
  assert(fabs(degree_to_radian(missing_angle.y)) < settings.aim_fov);
  float multiplier = angle_multiplier;
  if (csgo.isScoped(mem.local_player_addr))
    multiplier = angle_multiplier_scoped;
  float smooth = use_smooth ? settings.smoothing_factor : 1.0;
  // because format is {pitch, yaw, roll} the y and x have to be swapped
  int mouseAngle_x = static_cast<int>(-missing_angle.y * multiplier * inverse_sens * smooth);
  int mouseAngle_y = static_cast<int>(missing_angle.x * multiplier * inverse_sens * smooth);
  return {mouseAngle_x, mouseAngle_y};
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


pair<EntityType*, Vector> Aimer::findTargetDispatcher(Vector view) {
  Weapon weapon = csgo.getWeapon(mem.local_player_addr);
  switch(weapon) {
  case Weapon::ZEUS:
    return zeusTarget(view, settings.aim_fov);
  default:
    return closestTargetInFov(view, settings.aim_fov);
  }

}

/**
   like the default Target finding function.
   Except targets that are too far away.
   Not just those that are not in the FOV.
 */
pair<EntityType*, Vector> Aimer::zeusTarget(Vector view, float fov) {
  EntityType* local_player;
  int local_player_index = csgo.getLocalPlayerIndex();
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
  if (players.size() < 2)
    throw runtime_error("Only one player.");
  EntityType* closestPlayer = nullptr;
  float closestAngle = fov;
  Vector closestBone = {0, 0, 0};
  Team team = csgo.getTeam(mem.local_player_addr);
  if (settings.debug) cout << "Player count:" << players.size() << endl;
  if (settings.debug) cout << "Local player index:" << local_player_index << endl;
  int enemy_index = -1;
  for (EntityType* enemy : players) {
    enemy_index++;
    if (enemy_index == local_player_index || (!settings.aim_teammates && enemy->m_iTeamNum == team))
      continue;
    if (settings.smoke_check && csgo.lineThroughSmoke(player_pos, enemy->m_vecOrigin))
      continue;
    addr_type enemy_addr = csgo.getPlayerAddr(enemy);
    BoneInfo* boneMatrix = mem.getBoneMatrix(enemy_addr);
    for (unsigned int boneID : settings.bone_ids) {
      // cout << "bone ID:" << boneID << endl;
      try {
        bone_pos = {boneMatrix[boneID].y, boneMatrix[boneID].z, boneMatrix[boneID].x};
        // printf("bone: %f, %f, %f \n", bone_pos.x, bone_pos.z, bone_pos.z);
      } catch(const runtime_error& e) {
        if (settings.debug) cout << e.what() << endl;;
      }
      // find angle between player and target
      Vector distVec = getDist(&player_pos, &bone_pos);
      if (len(distVec) > ZEUS_RANGE)
        continue;
      normalize_vector(&distVec);
      float angle = acos(distVec * view);
      if (angle > fov / 2.)
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
    delete boneMatrix;
  }
  if (closestPlayer == nullptr)
    throw runtime_error("No player in FOV");
  // draw spherical hitboxes around the bone
  // and ignore adjustments if already aiming at part of this hitbox
  Vector distVec = getDist(&player_pos, &closestBone);
  Vector target = player_pos + view*len(distVec);
  if (lineSphereIntersection(player_pos, target, closestBone, BONE_RADIUS))
    throw runtime_error("No adjustment needed");
  return pair<EntityType*, Vector>(closestPlayer, closestBone);
}

pair<EntityType*, Vector> Aimer::closestTargetInFov(Vector view, float fov) {
  EntityType* local_player;
  int local_player_index = csgo.getLocalPlayerIndex();
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
  // unsigned int boneIds[] = {3, 6, 7, 8, 66, 67, 73, 74};
  if (players.size() < 2)
    throw runtime_error("Only one player.");
  EntityType* closestPlayer = nullptr;
  float closestAngle = fov;
  Vector closestBone = {0, 0, 0};
  Team team = csgo.getTeam(mem.local_player_addr);
  if (settings.debug) cout << "Player count:" << players.size() << endl;
  if (settings.debug) cout << "Local player index:" << local_player_index << endl;
  int enemy_index = -1;
  for (EntityType* enemy : players) {
    enemy_index++;
    if (enemy_index == local_player_index || (!settings.aim_teammates && enemy->m_iTeamNum == team))
      continue;
    if (settings.smoke_check && csgo.lineThroughSmoke(player_pos, enemy->m_vecOrigin))
      continue;
    addr_type enemy_addr = csgo.getPlayerAddr(enemy);
    BoneInfo* boneMatrix = mem.getBoneMatrix(enemy_addr);
    for (unsigned int boneID : settings.bone_ids) {
      // cout << "bone ID:" << boneID << endl;
      try {
        bone_pos = {boneMatrix[boneID].y, boneMatrix[boneID].z, boneMatrix[boneID].x};
        // printf("bone: %f, %f, %f \n", bone_pos.x, bone_pos.z, bone_pos.z);
      } catch(const runtime_error& e) {
        if (settings.debug) cout << e.what() << endl;;
      }
      // find angle between player and target
      Vector distVec = getDist(&player_pos, &bone_pos);
      normalize_vector(&distVec);
      float angle = acos(distVec * view);
      if (angle > fov / 2.)
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
    delete boneMatrix;
  }
  if (closestPlayer == nullptr)
    throw runtime_error("No player in FOV");
  // draw spherical hitboxes around the bone
  // and ignore adjustments if already aiming at part of this hitbox
  Vector distVec = getDist(&player_pos, &closestBone);
  Vector target = player_pos + view*len(distVec);
  if (lineSphereIntersection(player_pos, target, closestBone, BONE_RADIUS))
    throw runtime_error("No adjustment needed");
  return pair<EntityType*, Vector>(closestPlayer, closestBone);
}

Vector Aimer::predictPositionOffset(EntityType* player) {
  Vector vel = {player->m_vecVelocity.y, player->m_vecVelocity.z, player->m_vecVelocity.x};
  float t = (float) settings.main_loop_sleep / 1000.0;
  return vel * t;
}
