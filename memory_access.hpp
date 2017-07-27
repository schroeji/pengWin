#pragma once
#include <string>
class MemoryAccess {
private:
  const std::string GAME_NAME = "csgo_linux64";
  pid_t pid;
  uint64_t client_base;
  unsigned long int local_player_offset = 0x1c79b68;
  unsigned long int glow_offset = 0x2515360;

  void updateAddrs();

public:
  unsigned long int local_player_addr;
  unsigned long int local_player;
  unsigned long int glow_addr;

  pid_t getPid();
  uint64_t getModule(const std::string&);
  bool read(void*, void*, size_t);
  uint64_t getClientBase();
};
