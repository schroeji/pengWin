#ifndef MEMORY_ACCESS_H_
#define MEMORY_ACCESS_H_

#include "settings.hpp"
#include "typedef.hpp"
#include "util.hpp"
#include "vac_bypass.h"

#include <cstdint>
#include <string>
#include <vector>
class MemoryAccess {
private:
  const std::string GAME_NAME = "cs2";
  const std::string kernel_module_file_path = "/proc/read_access";
  pid_t pid;
  Settings *settings;
  std::vector<Addr_Range> engine_range;
  std::vector<Addr_Range> client_range;
  std::vector<Addr_Range> panorama_client_range;
  unsigned char *diffBuffer = nullptr;
  FILE *kernel_module_file;

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
  VacBypass vac_bypass;

  bool debug = false;
  void updateAddrs();

public:
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
  ~MemoryAccess();
  pid_t getPid();
  addr_type getEntityListAddr();
  std::vector<Addr_Range> getModule(const std::string &);
  std::vector<Addr_Range> getModuleFromKernelModule(const std::string &);
  bool read(void *, void *, size_t);
  bool read(addr_type, void *, size_t);
  bool write(void *, void *, size_t);
  unsigned int read_offset(void *);
  std::uint64_t read_uint64(void *);
  std::uint32_t read_uint32(void *);
  std::uint16_t read_uint16(void *);
  std::uint8_t read_uint8(void *);
  std::string read_string(void *);
  addr_type get_address(void *);
  std::vector<Addr_Range> getPanoramaClientRange();
  std::vector<Addr_Range> &getClientRange();
  std::vector<Addr_Range> getEngineRange();
  std::uint64_t getModuleSize(addr_type module_base);
  // returns the address of the call located at the first argument
  addr_type getCallAddress(void *);
  // returns the address of a jump
  addr_type getAbsoluteAddress(void *, int, int);

  void printAddrRangeVec(std::vector<Addr_Range> const &range_vec);
  void updateLocalPlayerAddr();
  BoneInfo *getBoneMatrix(addr_type);
  void printBlock(addr_type, size_t);
  std::vector<int> diffMem(addr_type, size_t);
};
#endif
