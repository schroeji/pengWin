#include "settings.hpp"
#include "typedef.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <X11/keysymdef.h>

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
      // remove comments
      splits = split_string(line, "#");
      //
      if (splits[0] == "")
        continue;
      else {
        splits = split_string(splits[0], "=");
        if (splits.size() != 2) {
          cout << "WARNING: invalid line in settings file:" << endl;
          cout << line << endl;
          continue;
        }
        else if (splits[0] == "glow_offset")
          glow_offset = strtoul(splits[1].c_str(), NULL, 16);
        else if (splits[0] == "attack_offset")
          attack_offset = strtoul(splits[1].c_str(), NULL, 16);
        else if (splits[0] == "local_player_offset")
          local_player_offset = strtoul(splits[1].c_str(), NULL, 16);
        else if (splits[0] == "map_name_offset")
          map_name_offset = strtoul(splits[1].c_str(), NULL, 16);
        else if (splits[0] == "main_loop_sleep")
          main_loop_sleep = strtol(splits[1].c_str(), NULL, 10);
        else if (splits[0] == "trigger_delay")
          trigger_delay = strtol(splits[1].c_str(), NULL, 10);
        else if (splits[0] == "trigger_use_random")
          trigger_use_random = (splits[1] == "true");
        else if (splits[0] == "trigger_key") {
          trigger_key = XStringToKeysym(splits[1].c_str());
          if (!trigger_key)
            cout << "WARNING: invalid trigger_key" << endl;
        }
        else if (splits[0] == "find_map")
          find_map = (splits[1] == "true");
        else if (splits[0] == "debug")
          debug = (splits[1] == "true");
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

  cout << "--------- Settings ---------" << endl;
  cout << dec << "main_loop_sleep: " << main_loop_sleep << endl;
  cout << dec << "trigger_delay: " << trigger_delay << endl;
  cout << dec << "trigger_use_random: " << trigger_use_random << endl;
  cout << dec << "find_map: " << find_map << endl;
  cout << "------------------------" << endl;
}
