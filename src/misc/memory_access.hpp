#pragma once
#include "settings.hpp"
#include "typedef.hpp"
#include "util.hpp"

#include <optional>
#include <string>
#include <vector>
class MemoryAccess {
private:
  const std::string GAME_NAME = "csgo_linux64";
  pid_t pid;
  Settings *settings;
  Addr_Range engine_range;
  Addr_Range client_range;
  unsigned char *diffBuffer = nullptr;

  addr_type local_player_addr_location;
  addr_type attack_addr_call_location;

  // offsets to client base
  addr_type local_player_offset;
  addr_type glow_offset;
  addr_type attack_offset;
  addr_type force_jump_offset;

  // offsets to engine base
  addr_type map_name_offset;
  addr_type isConnected_offset;
  addr_type clientState_offset;

  bool debug = false;
  void updateAddrs();

public:
  using BytePattern = std::vector<std::optional<char>>;

  // addreses in client
  addr_type local_player_addr;
  addr_type glow_addr;
  addr_type attack_addr;
  addr_type force_jump_addr;

  // addresses in engine
  addr_type map_name_addr;
  addr_type isConnected_addr;
  addr_type clientState_addr;

  // offsets to players
  addr_type m_iCrosshairIndex = 0xbbe4;
  addr_type m_iTeamNum;
  addr_type m_dwBoneMatrix;
  addr_type m_bIsScoped;
  addr_type m_Local;
  addr_type m_bIsDefusing;
  addr_type m_hActiveWeapon;
  addr_type m_flFlashDuration;
  // ofsets to player + m_Local
  addr_type m_viewPunchAngle;
  addr_type m_aimPunchAngle;
  addr_type m_AttributeManager;
  addr_type m_iItemDefinitionIndex;

  // offsets to ClientState
  // addr_type viewAngles = 0x8E20;

  MemoryAccess(Settings *);
  pid_t getPid();
  Addr_Range getModule(const std::string &);
  bool read(void *, void *, size_t);
  bool write(void *, void *, size_t);
  Addr_Range getClientRange();
  Addr_Range getEngineRange();
  // returns the address of the call located at the first argument
  addr_type getCallAddress(void *);
  // returns the address of a jump
  addr_type getAbsoluteAddress(void *, int, int);
  std::vector<addr_type> find_pattern(BytePattern pattern, Addr_Range range);
  void updateLocalPlayerAddr();
  BoneInfo *getBoneMatrix(addr_type);
  void printBlock(addr_type, size_t);
  std::vector<int> diffMem(addr_type, size_t);

  BytePattern compile_byte_pattern(const std::string &);
  BytePattern compile_byte_pattern(std::uint32_t);
};
