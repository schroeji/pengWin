#pragma once
#include "typedef.hpp"
#include "settings.hpp"

#include <string>
class MemoryAccess {
private:
  const std::string GAME_NAME = "csgo_linux64";
  pid_t pid;
  Addr_Range engine_range;
  Addr_Range client_range;

  addr_type local_player_addr_location;
  addr_type attack_addr_call_location;

  //offsets to client base
  addr_type local_player_offset;
  addr_type glow_offset;
  addr_type attack_offset;

  //offsets to engine base
  addr_type map_name_offset = 0x12177a5;

  //offsets to localPlayer
  addr_type crosshair_id_offset = 0xBBD8;
  addr_type team_number_offset = 0x128;

  void updateAddrs();

public:
  addr_type local_player_addr;
  addr_type glow_addr;
  addr_type attack_addr;
  addr_type map_name_addr;

  MemoryAccess(Settings*);
  pid_t getPid();
  Addr_Range getModule(const std::string&);
  bool read(void*, void*, size_t);
  bool write(void*, void*, size_t);
  Addr_Range getClientRange();
  Addr_Range getEngineRange();
  unsigned int getCrosshairTarget();
  addr_type getCallAddress(void*);
  addr_type getEngp();
  Team getTeam();
  std::string getMapName();
  addr_type find_pattern(const char*, const char*, Addr_Range);
};
