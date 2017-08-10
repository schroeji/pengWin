#pragma once
#include "typedef.hpp"

#include <string>

using namespace std;
class Settings {
private:
  static Settings* instance;

public:
  // offsets
  addr_type glow_offset;
  addr_type attack_offset;
  addr_type local_player_offset;

  // main settings
  long int main_loop_sleep;

  // trigger settings
  long int trigger_delay;
  bool trigger_use_random = false;

  //radar settings
  bool find_map = false;

  Settings(const string&);
  static Settings getInstance();
  void load(const string&);
};
