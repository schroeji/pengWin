#pragma once
#include "util.hpp"

#include <X11/Xlib.h>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

using namespace std;
class Settings {
private:
  static Settings *instance;

public:
  // main settings
  long int main_loop_sleep;
  bool debug = false;
  string mouse_file;
  string panic_key;
  string username;
  string maps_path;

  // trigger settings
  long int trigger_delay;
  bool trigger_use_random = false;
  string trigger_key;

  // bhop settings
  string bhop_key;

  // radar settings
  bool find_map = false;
  long int radar_sleep = 200;
  bool radar_generic = false;

  // aimbot settings
  float aim_fov;
  long int aim_sleep;
  string aim_key;
  float smoothing_factor = 0.2;
  float sensitivity;
  float aim_fov_max_scale = 1.0;
  bool aim_autoshoot = false;
  bool aim_smooth_first_shot = true;
  bool aim_vis_check;
  bool aim_smoke_check;
  bool aim_flash_check;
  bool aim_teammates;
  bool use_fake_input_device = false;
  bool use_kernel_module = true;

  map<Weapon, float> weapon_fovs;
  vector<unsigned int> bone_ids;
  std::map<std::string, std::uint64_t> netvars;

  Settings(const string &);
  static Settings &getInstance();
  void readNetvars(std::ifstream &handle);
  void load(const string &);
  void print();
};
