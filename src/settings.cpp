#include "settings.hpp"
#include "typedef.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <X11/keysymdef.h>
#include <stdlib.h>

using namespace std;
Settings* Settings::instance = nullptr;
Settings::Settings(const string& file) {
  load(file);
  instance = this;
}

Settings Settings::getInstance() {
  return *instance;
}

void Settings::load(const string& file) {
  ifstream settings_file (file);
  string line;
  vector<string> splits;
  if (settings_file.is_open()) {
    while (getline(settings_file, line)) {
      // remove comments in file
      splits = split_string(line, "#");
      if (splits[0] == "")
        continue;
      else {
        splits = split_string(splits[0], "=");
        if (splits.size() != 2) {
          cout << "WARNING: invalid line in settings file:" << endl;
          cout << line << endl;
          continue;
        }
        // offsets
        else if (splits[0] == "glow_offset")
          glow_offset = strtoul(splits[1].c_str(), NULL, 16);
        else if (splits[0] == "attack_offset")
          attack_offset = strtoul(splits[1].c_str(), NULL, 16);
        else if (splits[0] == "local_player_offset")
          local_player_offset = strtoul(splits[1].c_str(), NULL, 16);
        else if (splits[0] == "map_name_offset")
          map_name_offset = strtoul(splits[1].c_str(), NULL, 16);
        else if (splits[0] == "force_jump_offset")
          force_jump_offset = strtoul(splits[1].c_str(), NULL, 16);
        // settings
        else if (splits[0] == "main_loop_sleep")
          main_loop_sleep = strtol(splits[1].c_str(), NULL, 10);
        else if (splits[0] == "mouse_file")
          mouse_file = splits[1];
        else if (splits[0] == "trigger_delay")
          trigger_delay = strtol(splits[1].c_str(), NULL, 10);
        else if (splits[0] == "trigger_use_random")
          trigger_use_random = (splits[1] == "true");
        else if (splits[0] == "trigger_key") {
          trigger_key = splits[1];
        } else if (splits[0] == "find_map")
          find_map = (splits[1] == "true");
        else if (splits[0] == "debug")
          debug = (splits[1] == "true");
        else if (splits[0] == "aim_fov")
          aim_fov = degree_to_radian(strtof(splits[1].c_str(), NULL));
        else if (splits[0] == "bhop_key") {
          bhop_key = splits[1];
        } else if (splits[0] == "aim_key") {
          aim_key = splits[1];
        } else if (splits[0] == "aim_sleep")
          aim_sleep = strtol(splits[1].c_str(), NULL, 10);
        else if (splits[0] == "smoothing_factor")
          smoothing_factor = strtof(splits[1].c_str(), NULL);
        else if (splits[0] == "aim_autoshoot")
          aim_autoshoot = (splits[1] == "true");
        else if (splits[0] == "aim_smooth_first_shot")
          aim_smooth_first_shot = (splits[1] == "true");
      }
    }
  }
  settings_file.close();
}

void Settings::print() {
  cout << "--------- Offsets ---------" << endl;
  cout << hex << "glow offset: " << glow_offset << endl;
  cout << "attack offset: " << attack_offset << endl;
  cout << "local player offset: " << local_player_offset << endl;
  cout << "map_name_offset: " << map_name_offset << endl;
  cout << "force_jump_offset: " << force_jump_offset << endl;

  cout << "--------- Settings ---------" << endl;
  cout << dec << "main_loop_sleep: " << main_loop_sleep << endl;
  cout << "Mouse file: " << mouse_file << endl;
  cout << "trigger_delay: " << trigger_delay << endl;
  cout << "trigger_use_random: " << trigger_use_random << endl;
  cout << "Trigger Key: " << trigger_key << endl;
  cout << "Bhop Key: " << bhop_key << endl;
  cout << "Aim Key: " << aim_key << endl;
  cout << "Aimbot FOV: " << radian_to_degree(aim_fov) << endl;
  cout << "aim_sleep: " << aim_sleep << endl;
  cout << "aim_autoshoot: " << aim_autoshoot << endl;
  cout << "aim_smooth_first_shot: " << aim_smooth_first_shot << endl;
  cout << "smoothing_factor: " << smoothing_factor << endl;
  cout << "find_map: " << find_map << endl;
  cout << "------------------------" << endl;
}
