#include "pattern_scanner.h"
#include "util.hpp"
#include <cstdint>

addr_type PatternScanner::getLocalPlayerController() {
  if (local_player_controller_PP == 0) {
    PatternScanner::BytePattern localPlayerControllerPP_pattern{
        compile_byte_pattern("48 83 3D ?? ?? ?? ?? ?? 0F 95 C0 C3")};
    std::vector<addr_type> local_player_controller_match =
        find_pattern_vec(localPlayerControllerPP_pattern, mem.getClientRange());
    local_player_controller_PP =
        mem.getAbsoluteAddress((void *)local_player_controller_match[0], 3, 5);
  }
  return mem.get_address((void *)(local_player_controller_PP));
}

addr_type PatternScanner::getEntitySystem() {
  if (entity_system_PP == 0) {
    PatternScanner::BytePattern entitySystem_pattern{
        compile_byte_pattern("48 81 EC ?? ?? ?? ?? 4C 8D 2D ?? ?? ?? ?? EB")};
    std::vector<addr_type> entity_system_match =
        find_pattern_vec(entitySystem_pattern, mem.getClientRange());
    entity_system_PP =
        mem.getAbsoluteAddress((void *)entity_system_match[0], 10, 4);
  }
  return mem.get_address((void *)(entity_system_PP));
}

addr_type PatternScanner::getHealthOffset() {
  if (health_offset == 0) {
    PatternScanner::BytePattern pattern{
        compile_byte_pattern("C7 87 ?? ?? ?? ?? 00 00 00 00 48 8D 35")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());
    health_offset = mem.read_offset((void *)(match[0] + 2));
    cout << "Health offset: " << health_offset << endl;
  }
  return health_offset;
}

addr_type PatternScanner::getIsDefusingOffset() {
  if (is_defusing_offset == 0) {
    PatternScanner::BytePattern pattern{
        compile_byte_pattern("78 ?? 00 75 ?? 80 BB ?? ?? ?? ??")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());
    is_defusing_offset = mem.read_offset((void *)(match[0] + 7));
    cout << "IsDefusing offset: " << health_offset << endl;
  }
  return is_defusing_offset;
}

addr_type PatternScanner::getWeaponServicesOffset() {
  if (weapon_services_offset == 0) {
    PatternScanner::BytePattern pattern{compile_byte_pattern(
        "48 8B BE ?? ?? ?? ?? 48 8D 35 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 89 C2")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());
    weapon_services_offset = mem.read_offset((void *)(match[0] + 3));
    cout << "WeaponServices offset: " << weapon_services_offset << endl;
  }
  return active_weapon_offset;
}

addr_type PatternScanner::getActiveWeaponOffset() {
  if (active_weapon_offset == 0) {
    PatternScanner::BytePattern pattern{
        compile_byte_pattern("8B 57 ?? 83 FA FF 74 ?? 4C 8B 0D")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());

    std::uint8_t offset{};
    mem.read((void *)(match[0] + 2), &offset, sizeof(offset));
    active_weapon_offset = offset;
    cout << "ActiveWeapon offset: " << active_weapon_offset << endl;
  }
  return active_weapon_offset;
}

addr_type PatternScanner::getEntityListOffset() {
  if (entity_list_offset == 0) {
    PatternScanner::BytePattern pattern{compile_byte_pattern(
        "4C 8D 6F ?? 41 54 53 48 89 FB 48 83 EC ?? 48 89 07 48")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());
    std::uint8_t offset{};
    mem.read((void *)(match[0] + 3), &offset, sizeof(offset));
    entity_list_offset = offset;
    cout << "Entity list offset: " << entity_list_offset << endl;
  }
  return entity_list_offset;
}

addr_type PatternScanner::getPawnHandleOffset() {
  if (pawn_handle_offset == 0) {
    BytePattern pattern{compile_byte_pattern("84 C0 75 ?? 8B 8F ?? ?? ?? ??")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());
    pawn_handle_offset = mem.read_offset((void *)(match[0] + 6));
    cout << "Pawn handle offset: " << pawn_handle_offset << endl;
  }
  return pawn_handle_offset;
}

addr_type PatternScanner::getEntityListMaxIndexOffset() {
  if (entity_list_max_index_offset == 0) {
    BytePattern pattern{compile_byte_pattern("39 97 ?? ?? ?? ?? 7D 06")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());
    entity_list_max_index_offset = mem.read_offset((void *)(match[0] + 2));
    cout << "EntityList max index offset: " << entity_list_max_index_offset
         << endl;
  }
  return entity_list_max_index_offset;
}

addr_type PatternScanner::getGameSceneNodeOffset() {
  if (game_scene_node_offset == 0) {
    BytePattern pattern{compile_byte_pattern(
        "E8 ?? ?? ?? ?? 84 C0 75 ?? 49 8B BC 24 ?? ?? ?? ?? 4C 89 EE")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());
    game_scene_node_offset = mem.read_offset((void *)(match[0] + 13));
    cout << "Game scene node offset : " << game_scene_node_offset << endl;
  }
  return game_scene_node_offset;
}

addr_type PatternScanner::getAbsOriginOffset() {
  if (abs_origin_offset == 0) {
    BytePattern pattern{
        compile_byte_pattern("43 ?? F3 0F 10 83 ?? ?? ?? ?? 66")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());
    abs_origin_offset = mem.read_offset((void *)(match[0] + 6));
    cout << "Abs origin offset : " << abs_origin_offset << endl;
  }
  return abs_origin_offset;
}

addr_type PatternScanner::getTeamNumberOffset() {
  if (team_number_offset == 0) {
    BytePattern pattern{
        compile_byte_pattern("41 0F B6 84 24 ?? ?? ?? ?? 3C 03")};
    std::vector<addr_type> match =
        find_pattern_vec(pattern, mem.getClientRange());
    team_number_offset = mem.read_offset((void *)(match[0] + 5));
    cout << "Team number offset: " << team_number_offset << endl;
  }
  return team_number_offset;
}

std::vector<addr_type>
PatternScanner::find_pattern_vec(BytePattern pattern,
                                 std::vector<Addr_Range> ranges) {
  std::vector<addr_type> result{};
  for (auto range : ranges) {
    auto matches = find_pattern_range(pattern, range);
    result.insert(result.begin(), matches.begin(), matches.end());
  }
  if (result.empty()) {
    cout << "WARNING: no match for pattern " << endl;
  } else if (result.size() > 1) {
    cout << "WARNING: Multiple matches for pattern" << endl;
  }

  return result;
}

std::vector<addr_type> PatternScanner::find_pattern_range(BytePattern pattern,
                                                          Addr_Range range) {
  size_t begin = range.first;
  size_t end = range.second;
  char buffer[8192];
  size_t blocksize = sizeof(buffer);
  size_t totalsize = end - begin;
  size_t chunknum = 0;

  std::vector<addr_type> result{};
  while (totalsize > 0) {
    size_t readsize = min(totalsize, blocksize);
    size_t readaddr = begin + (blocksize * chunknum);
    std::fill_n(buffer, blocksize, 0);
    if (mem.read((void *)readaddr, buffer, readsize)) {
      for (size_t b = 0; b < readsize; b++) {
        size_t matches = 0;
        while (!pattern[matches].has_value() ||
               buffer[b + matches] == pattern[matches].value()) {
          matches++; // one matched byte
          if (matches == pattern.size()) {
            result.push_back(static_cast<addr_type>(readaddr + b));
            break;
          }
        }
      }
    }
    totalsize -= readsize;
    chunknum++;
  }
  return result;
}

PatternScanner::BytePattern
PatternScanner::compile_byte_pattern(const std::string &pattern) {
  std::vector<std::optional<char>> compiled_pattern{};
  for (size_t i = 0; i < pattern.size();
       i += 3) { // skip over two characters plus space
    if (pattern.substr(i, 2) == "??") {
      compiled_pattern.push_back(std::optional<char>{});
    } else {
      char byte = (char)strtol(pattern.substr(i, 2).c_str(), NULL, 16);
      compiled_pattern.push_back(std::optional<char>{byte});
    }
  }
  return compiled_pattern;
}

PatternScanner::BytePattern
PatternScanner::compile_byte_pattern(std::uint32_t pointer) {
  std::vector<std::optional<char>> compiled_pattern{};
  char *char_ptr = reinterpret_cast<char *>(&pointer);
  for (size_t i = 0; i < 4; i++) { // skip over two characters plus space
    compiled_pattern.push_back(std::optional<char>{char_ptr[i]});
  }
  return compiled_pattern;
}
