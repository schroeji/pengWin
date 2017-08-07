#include "settings.hpp"
#include "typedef.hpp"

#include <string>
#include <fstream>
#include <iostream>

using namespace std;
Settings* Settings::instance = nullptr;
Settings::Settings(const string& file) {
  load(file);
  instance = this;
}

Settings& Settings::getInstance() {
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
      }
    }
  }
}
