#include "settings.hpp"
#include "util.hpp"

#include <X11/keysymdef.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;
Settings *Settings::instance = nullptr;
Settings::Settings(const string &file) {
  load(file);
  instance = this;
}

Settings &Settings::getInstance() { return *instance; }

void Settings::load(const string &file) {
  ifstream settings_file(file);
  string line;
  vector<string> splits;
  vector<string> name_splits;

  if (settings_file.is_open()) {
    while (getline(settings_file, line)) {
      // remove comments in file
      splits = split_string(line, "#");
      if (splits[0] == "" || splits[0] == "[offsets]" ||
          splits[0] == "[settings]")
        continue;
      else if (splits[0] == "[netvars]") {
        readNetvars(settings_file);
        break;
      } else {
        splits = split_string(splits[0], "=");
        if (splits.size() != 2) {
          cout << "WARNING: invalid line in settings file:" << endl;
          cout << line << endl;
          continue;
        }
        name_splits = split_string(splits[0], "_");
        // settings
        if (splits[0] == "main_loop_sleep")
          main_loop_sleep = strtol(splits[1].c_str(), NULL, 10);
        else if (splits[0] == "mouse_file")
          mouse_file = splits[1];
        else if (splits[0] == "trigger_delay")
          trigger_delay = strtol(splits[1].c_str(), NULL, 10);
        else if (splits[0] == "trigger_use_random")
          trigger_use_random = (splits[1] == "true");
        else if (splits[0] == "trigger_key")
          trigger_key = splits[1];
        else if (splits[0] == "find_map")
          find_map = (splits[1] == "true");
        else if (splits[0] == "radar_sleep")
          radar_sleep = strtol(splits[1].c_str(), NULL, 10);
        else if (splits[0] == "radar_generic")
          radar_generic = (splits[1] == "true");
        else if (splits[0] == "debug")
          debug = (splits[1] == "true");
        else if (splits[0] == "aim_fov")
          aim_fov = degree_to_radian(strtof(splits[1].c_str(), NULL));
        else if (splits[0] == "bhop_key")
          bhop_key = splits[1];
        else if (splits[0] == "aim_key")
          aim_key = splits[1];
        else if (splits[0] == "aim_sleep")
          aim_sleep = strtol(splits[1].c_str(), NULL, 10);
        else if (splits[0] == "smoothing_factor")
          smoothing_factor = strtof(splits[1].c_str(), NULL);
        else if (splits[0] == "sensitivity")
          sensitivity = strtof(splits[1].c_str(), NULL);
        else if (splits[0] == "aim_autoshoot")
          aim_autoshoot = (splits[1] == "true");
        else if (splits[0] == "aim_smooth_first_shot")
          aim_smooth_first_shot = (splits[1] == "true");
        else if (splits[0] == "aim_smoke_check")
          aim_smoke_check = (splits[1] == "true");
        else if (splits[0] == "aim_flash_check")
          aim_flash_check = (splits[1] == "true");
        else if (splits[0] == "aim_fov_max_scale")
          aim_fov_max_scale = strtof(splits[1].c_str(), NULL);
        else if (splits[0] == "aim_vis_check")
          aim_vis_check = (splits[1] == "true");
        else if (splits[0] == "aim_teammates")
          aim_teammates = (splits[1] == "true");
        else if (splits[0] == "use_fake_input_device")
          use_fake_input_device = (splits[1] == "true");
        else if (splits[0] == "use_kernel_module")
          use_kernel_module = (splits[1] == "true");
        else if (splits[0] == "bone_ids") {
          vector<string> bones = split_string(splits[1], ",");
          for (string bone : bones)
            bone_ids.push_back(stoi(bone));
        } else if (splits[0] == "panic_key") {
          panic_key = splits[1];
        } else if (splits[0] == "user") {
          username = splits[1];
        } else {
          // weapon specific fov settings
          if (name_splits[0] == "aim" && name_splits[2] == "fov") {
            Weapon w = getWeaponByName(name_splits[1]);
            weapon_fovs[w] = degree_to_radian(strtof(splits[1].c_str(), NULL));
          }
        }
      }
    }
  } else {
    cout << "Error could not open " << file
         << ". Please run offset_dumper first." << endl;
  }
  settings_file.close();
}

void Settings::print() {
  // cout << "--------- NetVars ---------" << endl;
  // for (std::pair<std::string, std::uint64_t> const& netvar : netvars) {
  // cout << hex << netvar.first << "=0x" << netvar.second << endl;
  // }
  cout << "--------- Settings ---------" << endl;
  cout << "[General]" << endl;
  cout << dec << "main_loop_sleep: " << main_loop_sleep << endl;
  cout << "Mouse file: " << mouse_file << endl;
  cout << "Maps path: " << maps_path << endl;
  cout << "trigger_delay: " << trigger_delay << endl;
  cout << "trigger_use_random: " << trigger_use_random << endl;
  cout << "sensitivity: " << sensitivity << endl;

  cout << endl << "[Keys]" << endl;
  cout << "Trigger Key: " << trigger_key << endl;
  cout << "Bhop Key: " << bhop_key << endl;
  cout << "Aim Key: " << aim_key << endl;
  cout << "Panic Key: " << panic_key << endl;

  cout << endl << "[Aimbot]" << endl;
  cout << "smoothing_factor: " << smoothing_factor << endl;
  cout << "aim_fov: " << radian_to_degree(aim_fov) << endl;
  cout << "aim_sleep: " << aim_sleep << endl;
  cout << "aim_autoshoot: " << aim_autoshoot << endl;
  cout << "aim_smooth_first_shot: " << aim_smooth_first_shot << endl;
  cout << "aim_smoke_check: " << aim_smoke_check << endl;
  cout << "aim_flash_check: " << aim_flash_check << endl;
  cout << "aim_vis_check: " << aim_vis_check << endl;
  cout << "aim_teammates: " << aim_teammates << endl;
  cout << "aim_fov_max_scale: " << aim_fov_max_scale << endl;
  cout << "bone_ids: ";
  for (unsigned int bone : bone_ids)
    cout << bone << " ";
  cout << endl;
  cout << "Weapon fovs:" << endl;
  for (pair<Weapon, float> p : weapon_fovs)
    cout << "aim_" << getWeaponName(p.first)
         << "_fov: " << radian_to_degree(p.second) << endl;

  cout << endl << "[Radar]" << endl;
  cout << "find_map: " << find_map << endl;
  cout << "radar_sleep: " << radar_sleep << endl;
  cout << "radar_generic: " << radar_generic << endl;
  cout << "------------------------" << endl;
}

void Settings::readNetvars(ifstream &handle) {
  string line;
  vector<string> splits;
  while (getline(handle, line)) {
    splits = split_string(line, "#");
    splits = split_string(splits[0], "=");
    std::string qualified_name = splits[0];
    std::uint64_t value = strtoul(splits[1].c_str(), NULL, 16);
    netvars.insert(std::make_pair(qualified_name, value));
  }
}
