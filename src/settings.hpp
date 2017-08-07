#pragma once
#include "typedef.hpp"

#include <string>

using namespace std;
class Settings {
private:
  static Settings* instance;

public:
  addr_type glow_offset;
  addr_type attack_offset;
  addr_type local_player_offset;

  Settings(const string&);
  static Settings& getInstance();
  void load(const string&);
};
