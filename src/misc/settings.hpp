#pragma once
#include "typedef.hpp"

#include <string>
#include <X11/Xlib.h>

using namespace std;
class Settings {
private:
  static Settings* instance;

public:
  // offsets
  addr_type glow_offset;
  addr_type attack_offset;
  addr_type local_player_offset;
  addr_type map_name_offset;
  addr_type force_jump_offset;

  // main settings
  long int main_loop_sleep;
  bool debug = false;
  string mouse_file = "/dev/input/event0";

  // trigger settings
  long int trigger_delay;
  bool trigger_use_random = false;
  string trigger_key;

  //bhop settings
  string bhop_key;

  //radar settings
  bool find_map = false;
  long int radar_sleep = 200;

  //aimbot settings
  float aim_fov;
  long int aim_sleep;
  string aim_key;
  float smoothing_factor = 0.2;
  bool aim_autoshoot = false;
  bool aim_smooth_first_shot = true;

  Settings(const string&);
  static Settings getInstance();
  void load(const string&);
  void print();
};
