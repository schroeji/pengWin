#include <array>
#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

#include "../misc/pattern_scanner.h"
#include "../misc/settings.hpp"

using namespace std;

class DlHandle {
public:
  DlHandle() = delete;
  DlHandle(DlHandle const &) = delete;
  DlHandle(void *handle) : handle_{handle} {};
  ~DlHandle() { dlclose(handle_); }
  void *Get() const { return handle_; }

private:
  void *handle_;
};

std::map<std::string, DlHandle> load_modules() {
  std::string const base_path{"/home/hidden/.local/share/Steam/steamapps/"
                              "common/Counter-Strike Global Offensive/game/"};
  // std::string const base_path{"./"};
  static std::array<std::string, 17> const m_modules_path = {
      "bin/linuxsteamrt64/libsteam_api.so",
      "bin/linuxsteamrt64/libphonon.so",
      "bin/linuxsteamrt64/libv8_libbase.so",
      "bin/linuxsteamrt64/libv8_icuuc.so",
      "bin/linuxsteamrt64/libv8_icui18n.so",
      "bin/linuxsteamrt64/libv8.so",
      "bin/linuxsteamrt64/libavutil.so.56",
      "bin/linuxsteamrt64/libvpx.so.6",
      "bin/linuxsteamrt64/libavcodec.so.58",
      "bin/linuxsteamrt64/libavformat.so.58",
      "bin/linuxsteamrt64/libavresample.so.4",
      "bin/linuxsteamrt64/libswscale.so.5",
      "bin/linuxsteamrt64/libtier0.so",
      "bin/linuxsteamrt64/libvideo.so",
      "csgo/bin/linuxsteamrt64/libclient.so",
      "bin/linuxsteamrt64/libsteamnetworkingsockets.so",
      "bin/linuxsteamrt64/libengine2.so"};
  std::map<std::string, DlHandle> result{};
  for (string const &mod : m_modules_path) {
    std::filesystem::path path{base_path + mod};
    void *address = dlopen(path.string().c_str(), RTLD_NOW | RTLD_GLOBAL);
    char *error = dlerror();
    if (error != nullptr) {
      printf("Failed to open %s error: %s \n", path.string().c_str(), error);
    } else {
      printf("Opened %s at %lu\n", path.string().c_str(), address);
      result.emplace(path.filename().string(), address);
    }
  }
  return result;
}

// ClientClass load_module(std::string const &module_name) {
//   auto const handle_map = load_modules();
//   DlHandle const &module_handle{handle_map.at(module_name)};
//   using create_interface_t = void *(*)(const char *, int *);
//   create_interface_t create_interface_ptr =
//       reinterpret_cast<create_interface_t>(
//           dlsym(module_handle.Get(), "CreateInterface"));
//   printf("CreateInterface location: %lu\n", &create_interface_ptr);
//   void *client_interface = create_interface_ptr("VClient018", nullptr);
//   printf("Client interface location: %lu\n", &client_interface);
//   ClientClass *cc =
//       reinterpret_cast<i_baseclientdll
//       *>(client_interface)->get_all_classes();
//   return *cc;
// }

void write_netvars(std::map<std::string, std::uint64_t> const &netvars,
                   string const &file_name) {
  ofstream file(file_name, ios_base::app);
  file << endl;
  file << "[netvars]" << endl;
  for (std::pair<std::string, std::uint64_t> const &netvar : netvars) {
    file << netvar.first << "=0x" << hex << netvar.second << endl;
  }
  file << dec;
  file.close();
}

void write_offsets(vector<string> names, vector<string> offsets,
                   const string &file_name) {
  if (names.size() != offsets.size()) {
    cout << "offset and name length not matching" << endl;
  }
  ofstream file(file_name);
  file << "[offsets]" << endl;
  for (size_t i = 0; i < names.size(); i++) {
    file << names[i] << "=" << offsets[i] << endl;
  }
  file.close();
}

void print_offsets(vector<string> names, vector<string> offsets) {
  cout << "--------- Found Offsets ---------" << endl;
  for (size_t i = 0; i < names.size(); i++) {
    cout << names[i] << "=" << offsets[i] << endl;
  }
}

string read_settings(const string &file_name) {
  ifstream settings_file(file_name);
  string line;
  bool reading_settings = false;
  // find length
  settings_file.seekg(0, settings_file.end);
  // go back to start
  settings_file.seekg(0, settings_file.beg);
  int settings_start = 0;

  int settings_end = 0;

  if (settings_file.is_open()) {
    while (getline(settings_file, line)) {
      if (line == "[settings]") {
        reading_settings = true;
        settings_start = settings_file.tellg();
      } else if (line == "[netvars]") {
        settings_end = settings_file.tellg() - static_cast<std::streampos>(11);
        break;
      }
    }
  } else {
    cout << "No settings file found!" << endl;
    return "";
  }
  char *buffer = new char[settings_end - settings_start];
  if (reading_settings) {
    settings_file.seekg(settings_start);
    settings_file.read(buffer, settings_end - settings_start);
  }
  return "\n[settings]\n" + string(buffer);
}

void iterate_entity_list(MemoryAccess &mem,
                         CConcreteEntityList const &entity_list,
                         int highest_entity_index) {
  for (int i = 0; i <= highest_entity_index; ++i) {
    const auto chunkIndex =
        i / CConcreteEntityList::kNumberOfIdentitiesPerChunk;
    void *chunk = entity_list.chunks[chunkIndex];
    if (!chunk)
      continue;

    const auto indexInChunk =
        i % CConcreteEntityList::kNumberOfIdentitiesPerChunk;
    CConcreteEntityList::EntityChunk entity_chunk{};
    mem.read(chunk, &entity_chunk, sizeof(entity_chunk));
    if (const auto &entityIndentity = entity_chunk[indexInChunk];
        entityIndentity.entity && (entityIndentity.handle & 0x7fff) == i)
      cout << "Found entity:" << i << endl;
  }
}

void write_settings(const string &file_name) {
  ofstream file(file_name, ios_base::app);
  file << endl;
  file << "[settings]" << endl;
  file << "# general settings" << endl;
  file << "main_loop_sleep=10" << endl;
  file << "debug=false" << endl;
  file << "panic_key=F9" << endl;
  file << "# file for reading mouse clicks" << endl;
  file << "# usually /dev/input/event*" << endl;
  file << "mouse_file=/dev/input/event2" << endl;
  file << "maps_path=~/.local/share/Steam/steamapps/common/Counter-Strike "
          "Global Offensive/csgo/maps/"
       << endl;
  file << "# trigger settings" << endl;
  file << "trigger_delay=10" << endl;
  file << "trigger_use_random=false" << endl;
  file << "# find your key with \"$ dumpkeys -l\"" << endl;
  file << "trigger_key=Caps_Lock" << endl;
  file << "# bhop settings" << endl;
  file << "bhop_key=space" << endl;
  file << "# aimbot settings" << endl;
  file << "aim_key=mouse1" << endl;
  file << "aim_sleep=20" << endl;
  file << "aim_fov=1.9" << endl;
  file << "# you can set additional weapon specific fovs like this:" << endl;
  file << "# aim_deagle_fov=2.0" << endl;
  file << "# aim_m4a4_fov=1.5" << endl;
  file << "aim_autoshoot=false" << endl;
  file << "#bone_ids=3,4,5,6,7,8,10,11,12,13,14,39,40,41,42,70,71,72,77,78,79"
       << endl;
  file << "bone_ids=3,4,5,6,7,8" << endl;
  file << "# enable / disable smoothing on first shot you may want to disable "
          "this for perfect flicks"
       << endl;
  file << "aim_smooth_first_shot=true" << endl;
  file << "smoothing_factor=0.25" << endl;
  file << "aim_smoke_check=true" << endl;
  file << "aim_flash_check=true" << endl;
  file << "aim_vis_check=true" << endl;
  file << "aim_teammates=false" << endl;
  file << "# if this > 1 your fov will increase linearly to a maxium factor "
          "of "
          "aim_fov_max_scale over 3 seconds"
       << endl;
  file << "aim_fov_max_scale=3.1" << endl;
  file << "# your ingame sensitivity (used for aim calculations)" << endl;
  file << "sensitivity=1.3" << endl;
  file << "use_fake_input_device=false" << endl;
  file << "# radar settings" << endl;
  file << "find_map=true" << endl;
  file << "radar_sleep=100" << endl;
  file << "radar_generic=false" << endl;
  file.close();
}

int main(int argc, char **argv) {
  if (getuid() != 0) {
    cout << "Not root" << endl;
    return 0;
  }
  // ClientClass cc = load_module("lib_client.so");
  // NetvarFinder netFinder(cc);
  // netFinder.dump();
  // std::map<std::string, std::uint64_t> const &netvars =
  // netFinder.getNetvars();

  const string file_name = "settings.cfg";
  string settings = "";
  bool retain_settings = true;
  if (argc > 1)
    if (!strcmp(argv[1], "--renew") || !strcmp(argv[1], "-r"))
      retain_settings = false;
  if (retain_settings)
    settings = read_settings(file_name);
  Settings sets{file_name};
  MemoryAccess mem{&sets};
  PatternScanner scanner{mem};
  mem.getPid();
  std::vector<Addr_Range> clientRange = mem.getClientRange();
  // std::vector<Addr_Range> panoramaClientRange = mem.getPanoramaClientRange();
  // Addr_Range engineRange = mem.getEngineRange();

  PatternScanner::BytePattern localPlayerControllerPP_pattern{
      scanner.compile_byte_pattern("48 83 3D ?? ?? ?? ?? ?? 0F 95 C0 C3")};
  PatternScanner::BytePattern playerPawnHandle_pattern{
      scanner.compile_byte_pattern("84 C0 75 ?? 8B 8F ?? ?? ?? ??")};
  PatternScanner::BytePattern healthoffset_pattern{
      scanner.compile_byte_pattern("C7 87 ?? ?? ?? ?? 00 00 00 00 48 8D 35")};
  PatternScanner::BytePattern playerColorOffset_pattern{
      scanner.compile_byte_pattern("49 63 9C 24 ?? ?? ?? ?? 83 FB 04")};
  PatternScanner::BytePattern viewMatrix_pattern{
      scanner.compile_byte_pattern("48 8D 0D ?? ?? ?? ?? 48 8B 38 E8")};
  PatternScanner::BytePattern entitySystem_pattern{scanner.compile_byte_pattern(
      "48 81 EC ?? ?? ?? ?? 4C 8D 2D ?? ?? ?? ?? EB")};
  PatternScanner::BytePattern entityList_offset_pattern{
      scanner.compile_byte_pattern(
          "4C 8D 6F ?? 41 54 53 48 89 FB 48 83 EC ?? 48 89 07 48")};
  PatternScanner::BytePattern highest_entity_index_offset_pattern{
      scanner.compile_byte_pattern("39 97 ?? ?? ?? ?? 7D 06")};
  cout << "Local player call" << endl;
  std::vector<addr_type> local_player_controller_match =
      scanner.find_pattern_vec(localPlayerControllerPP_pattern, clientRange);
  cout << "local_player call: " << local_player_controller_match[0] << endl;
  cout << "Player pawn handle" << endl;
  std::vector<addr_type> player_pawn_handle_offset =
      scanner.find_pattern_vec(playerPawnHandle_pattern, clientRange);
  cout << "Health offset" << endl;
  std::vector<addr_type> health_offset_match =
      scanner.find_pattern_vec(healthoffset_pattern, clientRange);
  cout << "Player color" << endl;
  std::vector<addr_type> player_color_offset_match =
      scanner.find_pattern_vec(playerColorOffset_pattern, clientRange);
  cout << "Entity system" << endl;
  std::vector<addr_type> entity_system_match =
      scanner.find_pattern_vec(entitySystem_pattern, clientRange);
  cout << "Entity list offset" << endl;
  std::vector<addr_type> entity_list_offset_match =
      scanner.find_pattern_vec(entityList_offset_pattern, clientRange);
  cout << "Highest entity index" << endl;
  std::vector<addr_type> highest_entity_index_offset_match =
      scanner.find_pattern_vec(highest_entity_index_offset_pattern,
                               clientRange);

  // std::vector<addr_type> view_matrix =
  //     mem.find_pattern_vec(viewMatrix_pattern, clientRange);
  auto local_player_PP =
      mem.getAbsoluteAddress((void *)local_player_controller_match[0], 3, 5);
  auto entity_system_PP =
      mem.getAbsoluteAddress((void *)entity_system_match[0], 10, 4);
  auto pawn_handle_offset =
      mem.read_offset((void *)(player_pawn_handle_offset[0] + 6));
  auto player_color_offset =
      mem.read_offset((void *)(player_color_offset_match[0] + 4));
  auto health_offset = mem.read_offset((void *)(health_offset_match[0] + 2));
  auto highest_entity_index_offset =
      mem.read_offset((void *)(highest_entity_index_offset_match[0] + 2));

  std::uint8_t entity_list_offset{};
  mem.read((void *)(entity_list_offset_match[0] + 3), &entity_list_offset,
           sizeof(entity_list_offset));
  auto local_player = mem.get_address((void *)(local_player_PP));
  auto entity_system = mem.get_address((void *)(entity_system_PP));
  auto entity_list_addr = entity_system + entity_list_offset;
  auto highest_entity_index_addr = entity_system + highest_entity_index_offset;
  auto highest_entity_index =
      mem.read_offset((void *)highest_entity_index_addr);

  cout << "local_player_PP: " << local_player_PP << endl;
  cout << "local_player: " << local_player << endl;
  cout << "pawn handle offset: " << pawn_handle_offset << endl;
  cout << "player color offset: " << player_color_offset << endl;
  cout << "entity_system_PP: " << entity_system_PP << endl;
  cout << "entity_system: " << entity_system << endl;
  cout << "entity_list_offset: " << entity_list_offset << endl;
  cout << "entity_list_addr: " << entity_list_addr << endl;
  cout << "highest_entity_index_addr: " << highest_entity_index_addr << endl;
  cout << "highest_entity_index: " << highest_entity_index << endl;

  mem.printBlock(local_player_PP, 32);
  mem.printBlock(entity_system_PP, 32);
  mem.printBlock(entity_list_offset_match[0], 32);

  addr_type player_pawn_handle_addr = local_player + pawn_handle_offset;
  auto player_pawn_handle = mem.read_offset((void *)player_pawn_handle_addr);

  // Get entity from player pawn handle
  auto player_pawn_index = player_pawn_handle & 0x7FFF;
  auto chunkIndex =
      player_pawn_index / CConcreteEntityList::kNumberOfIdentitiesPerChunk;
  CConcreteEntityList entity_list{};
  mem.read((void *)entity_list_addr, &entity_list, sizeof(entity_list));
  auto *chunk = entity_list.chunks[chunkIndex];
  const auto indexInChunk =
      player_pawn_index % CConcreteEntityList::kNumberOfIdentitiesPerChunk;
  CConcreteEntityList::EntityChunk entity_chunk{};
  mem.read(chunk, &entity_chunk, sizeof(entity_chunk));
  CEntityIdentity entityIdentity = entity_chunk[indexInChunk];

  if (entityIdentity.handle == player_pawn_handle) {
    cout << "Handles match" << endl;

  } else {
    cout << "chunk: " << chunk << endl;
    cout << "chunkIndex: " << chunkIndex << endl;
    cout << "entityIndentity.handle: " << entityIdentity.handle << endl;
    cout << "player_pawn_handle: " << player_pawn_handle << endl;
    cout << "player_pawn_index: " << player_pawn_index << endl;
  }

  unsigned int health;
  unsigned int player_color_index;
  mem.read((void *)(entityIdentity.entity + health_offset), &health,
           sizeof(health));
  mem.read((void *)(local_player + player_color_offset), &player_color_index,
           sizeof(player_color_index));
  cout << "Local player controller:" << local_player << endl;
  cout << "health offset: " << health_offset << endl;
  cout << "Player pawn handle:" << player_pawn_handle << endl;
  // cout << "View matrix" << view_matrix[0] << endl;
  cout << "health: " << health << endl;
  cout << "player color index: " << player_color_index << endl;

  mem.printBlock(local_player + player_color_offset, 32);

  // iterate_entity_list(mem, entity_list, highest_entity_index);
  // const char glowPointerCall_data[] =
  // "\xE8\x00\x00\x00\x00\x48\x8b\x3d\x00\x00"
  //                                     "\x00\x00\xBE\x01\x00\x00\x00\xC7";
  // const char glowPointerCall_pattern[] = "x????xxx????xxxxxx";
  // PatternScanner::BytePattern
  // glowPointerCall_pattern{mem.compile_byte_pattern(
  //     "E8 ?? ?? ?? ?? 48 8B 3D ?? ?? ?? ?? BE 01 00 00 00 C7")};
  // const string glowPointerCall_pattern = "E8 ?? ?? ?? ?? 49 8B 7D 00 C7 40
  //     38
  //     ? ? ? ? ? ? ? ? 48 8B 07 FF 90 ";
  //     PatternScanner::BytePattern local_player_addr_pattern{
  //         mem.compile_byte_pattern("48 89 e5 74 0e 48 8d 05 ?? ?? ?? ??")};

  // PatternScanner::BytePattern atk_mov_pattern{mem.compile_byte_pattern(
  //     "89 D8 83 C8 01 F6 C2 03 0F 45 D8 44 89 ?? 83 E0 01 F7 D8 83 E8
  //     03")};
  // const string map_name_pattern = "48 89 C7 44 89 9D B4 FE FF FF 48 8D 35
  //     ? ? ? ? ? ? ? ? ";
  //     PatternScanner::BytePattern map_name_pattern{mem.compile_byte_pattern(
  //         "44 89 9D B4 FE FF FF 48 8D 35 ?? ?? ?? ??")};

  // PatternScanner::BytePattern force_jump_pattern{mem.compile_byte_pattern(
  //     "44 89 e8 c1 e0 1d c1 f8 1f 83 e8 03 45 84 e4 74 08 21 d0")};
  // PatternScanner::BytePattern split_screen_pattern{mem.compile_byte_pattern(
  //     "55 89 FE 48 8D 3D ?? ?? ?? ?? 48 89 E5 5D E9 AD FF FF FF")};
  // PatternScanner::BytePattern
  // is_connected_move_pattern{mem.compile_byte_pattern(
  //     "48 8b 05 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 00 48 8b 10")};
  // PatternScanner::BytePattern client_state_pattern{mem.compile_byte_pattern(
  //     "48 8B 05 ?? ?? ?? ?? 55 48 8D 3D ?? ?? ?? ?? 48 89 E5 FF 50 28")};
  // PatternScanner::BytePattern hasC4_pattern{mem.compile_byte_pattern(
  //     "55 48 89 E5 41 54 53 48 89 FB E8 ? ? ? ? 84 C0 75 3D")};

  // // for finding netvars
  // PatternScanner::BytePattern dwGetAllClasses_pattern{
  //     mem.compile_byte_pattern("48 8B ?? ?? ?? ?? ?? 8B ?? ?? 48 ?? ?? 48
  //     ??
  //                              "
  //                              "?? 75 ?? e9 ?? ?? ?? ?? 66")};

  vector<string> offsets;
  vector<string> offset_names;
  char offset_buf[64];

  cout << "-- Glow Pointer --" << endl;
  // addr_type glowPointerCall =
  //     mem.find_pattern(glowPointerCall_pattern, clientRange)[0];
  // addr_type glowFunctionCall = mem.getCallAddress((void *)glowPointerCall);
  // unsigned int glowOffset;
  // mem.read((void*) (glowFunctionCall + 0x10), &glowOffset, sizeof(int));
  // addr_type glowManAddr = glowFunctionCall + 0x10 + glowOffset + 0x4;
  // addr_type glowManAddr =
  //     mem.getAbsoluteAddress((void *)(glowFunctionCall + 0x9), 3, 7);
  // addr_type glow_offset = glowManAddr - clientRange.first;
  // // addr_type glow_offset = glowManAddr;
  // sprintf(offset_buf, "0x%lx", glow_offset);
  // offset_names.push_back("glow_offset");
  // offsets.push_back(string(offset_buf));

  // cout << "-- Local Player --" << endl;
  // addr_type localPlayerFunction =
  //     mem.find_pattern(local_player_addr_pattern, clientRange)[0];
  // addr_type local_player_addr =
  //     mem.getCallAddress((void *)(localPlayerFunction + 0x7));
  // addr_type local_player_offset = local_player_addr - clientRange.first;
  // sprintf(offset_buf, "0x%lx", local_player_offset);
  // offset_names.push_back("local_player_offset");
  // offsets.push_back(string(offset_buf));

  // cout << "-- Attack --" << endl;
  // addr_type attack_addr = mem.find_pattern(atk_mov_pattern,
  // clientRange)[0]; addr_type atk_offset = attack_addr - clientRange.first;
  // sprintf(offset_buf, "0x%lx", atk_offset);
  // offset_names.push_back("attack_offset");
  // offsets.push_back(string(offset_buf));

  // cout << "-- Map Name --" << endl;
  // std::vector<addr_type> map_name_calls =
  //     mem.find_pattern(map_name_pattern, engineRange);
  // addr_type map_name_addr =
  //     mem.getCallAddress((void *)(map_name_calls[0] + 0x9));
  // // example: "maps/de_dust2.bsp"
  // addr_type map_name_offset =
  //     map_name_addr - engineRange.first + 0x5; // add 5 because of "maps/"
  // sprintf(offset_buf, "0x%lx", map_name_offset);
  // offset_names.push_back("map_name_offset");
  // offsets.push_back(string(offset_buf));

  // cout << "-- Client State --" << endl;
  // addr_type clientState_addr =
  //     mem.find_pattern(client_state_pattern, engineRange)[0];
  // addr_type clientState_offset = clientState_addr - engineRange.first;
  // sprintf(offset_buf, "0x%lx", clientState_offset);
  // offset_names.push_back("clientState_offset");
  // offsets.push_back(string(offset_buf));

  // // netvar dumping
  // cout << "-- dwGetAllClasses --" << endl;
  // addr_type allClasses_call =
  //     mem.find_pattern(dwGetAllClasses_pattern, clientRange)[0];
  // addr_type allClasses_addr =
  //     mem.getCallAddress((void *)(allClasses_call + 0x2));
  // mem.read((void *)(allClasses_addr), (void *)&allClasses_addr,
  //          sizeof(allClasses_addr));
  // mem.read((void *)(allClasses_addr), (void *)&allClasses_addr,
  //          sizeof(allClasses_addr));
  // for (std::pair<std::string, std::uint64_t> const& netvar : netvars) {
  //   cout << netvar.first << " = " << netvar.second << endl;
  // }

  // cout << "-- Force Jump --" << endl;
  // addr_type force_jump_call = mem.find_pattern(force_jump_data,
  // force_jump_pattern, clientRange); addr_type force_jump_addr =
  // mem.getCallAddress((void*) (force_jump_call + 0x1A)); addr_type
  // force_jump_offset = force_jump_addr - clientRange.first;
  // sprintf(offset_buf, "0x%lx", force_jump_offset);
  // offset_names.push_back("force_jump_offset");
  // offsets.push_back(string(offset_buf));

  // cout << "-- is Connected --" << endl;
  // addr_type isConnected_call = mem.find_pattern(isConnectedMove_pattern,
  // engineRange); addr_type isConnected_addr = mem.getCallAddress((void*)
  // (isConnected_call + 0x8)) + 1; addr_type isConnected_offset =
  // isConnected_addr - engineRange.first; sprintf(offset_buf,
  // "0x%lx",isConnected_offset);
  // offset_names.push_back("isConnected_offset");
  // offsets.push_back(string(offset_buf));

  // cout << "-- SplitScreen --" << endl;
  // addr_type split_call = mem.find_pattern(isConnectedMove_pattern,
  // engineRange); addr_type isConnected_addr = mem.getCallAddress((void*)
  // (isConnected_call + 0x8)) + 1; addr_type isConnected_offset =
  // isConnected_addr - engineRange.first; sprintf(offset_buf,
  // "0x%lx",isConnected_offset);
  // offset_names.push_back("isConnected_offset");
  // offsets.push_back(string(offset_buf));

  // cout << "Test1: " << test1 << endl;
  // cout << "Test2: " << test2 << endl;
  // write_offsets(offset_names, offsets, file_name);
  // print_offsets(offset_names, offsets);

  // if (retain_settings && settings != "") {
  //   // rewrite settings
  //   ofstream file(file_name, ios_base::app);
  //   file.write(settings.c_str(), settings.size());
  //   file.close();
  // } else {
  //   write_settings(file_name);
  // }

  // write_netvars(netvars, file_name);

  return 0;
}
