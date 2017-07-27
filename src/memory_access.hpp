#pragma once
#include "typedef.hpp"
#include <string>
class MemoryAccess {
private:
  const std::string GAME_NAME = "csgo_linux64";
  pid_t pid;
  unsigned long int client_base;
  unsigned long int local_player_addr;

  //offsets to client base
  unsigned long int local_player_offset = 0x1c79b68;
  unsigned long int glow_offset = 0x2515360;

  //offsets to localPlayer
  unsigned long int crosshair_id_offset = 0xBBD8;
  unsigned long int team_number_offset = 0x128;

  void updateAddrs();

public:
  unsigned long int local_player;
  unsigned long int glow_addr;

  pid_t getPid();
  unsigned long int getModule(const std::string&);
  bool read(void*, void*, size_t);
  unsigned long int getClientBase();
  unsigned int getCrosshairTarget();
  Team getTeam();
};
