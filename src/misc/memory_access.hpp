#pragma once
#include "typedef.hpp"
#include "settings.hpp"

#include <string>
class MemoryAccess {
private:
  const std::string GAME_NAME = "csgo_linux64";
  pid_t pid;
  Settings* settings;
  Addr_Range engine_range;
  Addr_Range client_range;

  addr_type local_player_addr_location;
  addr_type attack_addr_call_location;

  //offsets to client base
  addr_type local_player_offset;
  addr_type glow_offset;
  addr_type attack_offset;
  addr_type force_jump_offset;

  //offsets to engine base
  addr_type map_name_offset;

  void updateAddrs();
  bool debug = false;

public:
  addr_type local_player_addr;
  addr_type glow_addr;
  addr_type attack_addr;
  addr_type map_name_addr;
  addr_type force_jump_addr;

  //offsets to players
  addr_type m_iCrosshairIndex = 0xBBB8;
  addr_type m_iTeamNum = 0x12C;
  addr_type m_dwBoneMatrix = 0x2C70;
  addr_type m_bIsScoped = 0x4144;
  addr_type m_Local = 0x36f0;

  //ofsets to player+ m_Local
  addr_type m_viewPunchAngle = 0x68;
  addr_type m_aimPunchAngle = 0x74;

  MemoryAccess(Settings*);
  pid_t getPid();
  Addr_Range getModule(const std::string&);
  bool read(void*, void*, size_t);
  bool write(void*, void*, size_t);
  Addr_Range getClientRange();
  Addr_Range getEngineRange();
  addr_type getCallAddress(void*);
  addr_type find_pattern(const char*, const char*, Addr_Range);
  void updateLocalPlayerAddr();
  Vector getBone(addr_type, unsigned int);
};