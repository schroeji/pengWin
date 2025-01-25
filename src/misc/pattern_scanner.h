#ifndef PATTERN_SCANNER_H_
#define PATTERN_SCANNER_H_
#include "memory_access.hpp"
#include "util.hpp"
#include <cstdint>
#include <optional>

class PatternScanner {
public:
  PatternScanner(MemoryAccess &mem) : mem(mem) {}
  addr_type getLocalPlayerController();
  addr_type getGlobalVars();
  addr_type getEntitySystem();
  addr_type getHealthOffset();
  addr_type getEntityListOffset();
  addr_type getPawnHandleOffset();
  addr_type getEntityListMaxIndexOffset();
  addr_type getGameSceneNodeOffset();
  addr_type getTeamNumberOffset();
  addr_type getAbsOriginOffset();
  addr_type getIsDefusingOffset();
  addr_type getActiveWeaponOffset();
  addr_type getWeaponServicesOffset();

  using BytePattern = std::vector<std::optional<char>>;

  BytePattern compile_byte_pattern(const std::string &);
  BytePattern compile_byte_pattern(std::uint32_t);
  std::vector<addr_type> find_pattern_vec(BytePattern pattern,
                                          std::vector<Addr_Range> ranges);
  std::vector<addr_type> find_pattern_range(BytePattern pattern,
                                            Addr_Range range);

private:
  MemoryAccess &mem;
  addr_type local_player_controller_PP{0};
  addr_type entity_system_PP{0};
  addr_type global_vars_PP{0};
  // Offsets to entity_system
  addr_type entity_list_offset{0};
  addr_type entity_list_max_index_offset{0};
  // Offset to LocalPlayerController
  addr_type pawn_handle_offset{0};

  // Offset to BaseEntity
  addr_type game_scene_node_offset{0};
  addr_type team_number_offset{0};
  addr_type health_offset{0};
  addr_type is_defusing_offset{0};
  addr_type weapon_services_offset{0};

  // Offset to WeaponServices
  addr_type active_weapon_offset{0};

  // Offset to game scene node
  addr_type abs_origin_offset{0};
};

#endif // PATTERN_SCANNER_H_
