#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

#include "misc/typedef.hpp"
#include "netvar_finder.hpp"

using namespace std;

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
  file << "# if this > 1 your fov will increase linearly to a maxium factor of "
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
  const string file_name = "settings.cfg";
  string settings = "";
  bool retain_settings = true;
  if (argc > 1)
    if (!strcmp(argv[1], "--renew") || !strcmp(argv[1], "-r"))
      retain_settings = false;
  if (retain_settings)
    settings = read_settings(file_name);
  MemoryAccess mem(nullptr);
  mem.getPid();
  Addr_Range clientRange = mem.getClientRange();
  Addr_Range engineRange = mem.getEngineRange();
  cout << hex << "Client size: " << clientRange.second - clientRange.first
       << endl;
  cout << "Engine size: " << engineRange.second - engineRange.first << endl;
  // const char glowPointerCall_data[] =
  // "\xE8\x00\x00\x00\x00\x48\x8b\x3d\x00\x00\x00\x00\xBE\x01\x00\x00\x00\xC7";
  // const char glowPointerCall_pattern[] = "x????xxx????xxxxxx";
  MemoryAccess::BytePattern glowPointerCall_pattern{mem.compile_byte_pattern(
      "E8 ?? ?? ?? ?? 48 8B 3D ?? ?? ?? ?? BE 01 00 00 00 C7")};
  // const string glowPointerCall_pattern = "E8 ?? ?? ?? ?? 49 8B 7D 00 C7 40 38
  // ?? ?? ?? ?? 48 8B 07 FF 90";
  MemoryAccess::BytePattern local_player_addr_pattern{
      mem.compile_byte_pattern("48 89 e5 74 0e 48 8d 05 ?? ?? ?? ??")};

  MemoryAccess::BytePattern atk_mov_pattern{mem.compile_byte_pattern(
      "89 D8 83 C8 01 F6 C2 03 0F 45 D8 44 89 ?? 83 E0 01 F7 D8 83 E8 03")};
  // const string map_name_pattern = "48 89 C7 44 89 9D B4 FE FF FF 48 8D 35 ??
  // ?? ?? ??";
  MemoryAccess::BytePattern map_name_pattern{
      mem.compile_byte_pattern("44 89 9D B4 FE FF FF 48 8D 35 ?? ?? ?? ??")};

  MemoryAccess::BytePattern force_jump_pattern{mem.compile_byte_pattern(
      "44 89 e8 c1 e0 1d c1 f8 1f 83 e8 03 45 84 e4 74 08 21 d0")};
  MemoryAccess::BytePattern split_screen_pattern{mem.compile_byte_pattern(
      "55 89 FE 48 8D 3D ?? ?? ?? ?? 48 89 E5 5D E9 AD FF FF FF")};
  MemoryAccess::BytePattern is_connected_move_pattern{mem.compile_byte_pattern(
      "48 8b 05 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 00 48 8b 10")};
  MemoryAccess::BytePattern client_state_pattern{mem.compile_byte_pattern(
      "48 8B 05 ?? ?? ?? ?? 55 48 8D 3D ?? ?? ?? ?? 48 89 E5 FF 50 28")};
  MemoryAccess::BytePattern hasC4_pattern{mem.compile_byte_pattern(
      "55 48 89 E5 41 54 53 48 89 FB E8 ? ? ? ? 84 C0 75 3D")};

  // for finding netvars
  MemoryAccess::BytePattern dwGetAllClasses_pattern{
      mem.compile_byte_pattern("48 8B ?? ?? ?? ?? ?? 8B ?? ?? 48 ?? ?? 48 ?? "
                               "?? 75 ?? e9 ?? ?? ?? ?? 66")};

  vector<string> offsets;
  vector<string> offset_names;
  char offset_buf[64];

  cout << "-- Glow Pointer --" << endl;
  addr_type glowPointerCall =
      mem.find_pattern(glowPointerCall_pattern, clientRange)[0];
  addr_type glowFunctionCall = mem.getCallAddress((void *)glowPointerCall);
  // unsigned int glowOffset;
  // mem.read((void*) (glowFunctionCall + 0x10), &glowOffset, sizeof(int));
  // addr_type glowManAddr = glowFunctionCall + 0x10 + glowOffset + 0x4;
  addr_type glowManAddr =
      mem.getAbsoluteAddress((void *)(glowFunctionCall + 0x9), 3, 7);
  addr_type glow_offset = glowManAddr - clientRange.first;
  // addr_type glow_offset = glowManAddr;
  sprintf(offset_buf, "0x%lx", glow_offset);
  offset_names.push_back("glow_offset");
  offsets.push_back(string(offset_buf));

  cout << "-- Local Player --" << endl;
  addr_type localPlayerFunction =
      mem.find_pattern(local_player_addr_pattern, clientRange)[0];
  addr_type local_player_addr =
      mem.getCallAddress((void *)(localPlayerFunction + 0x7));
  addr_type local_player_offset = local_player_addr - clientRange.first;
  sprintf(offset_buf, "0x%lx", local_player_offset);
  offset_names.push_back("local_player_offset");
  offsets.push_back(string(offset_buf));

  cout << "-- Attack --" << endl;
  addr_type attack_addr = mem.find_pattern(atk_mov_pattern, clientRange)[0];
  addr_type atk_offset = attack_addr - clientRange.first;
  sprintf(offset_buf, "0x%lx", atk_offset);
  offset_names.push_back("attack_offset");
  offsets.push_back(string(offset_buf));

  cout << "-- Map Name --" << endl;
  std::vector<addr_type> map_name_calls =
      mem.find_pattern(map_name_pattern, engineRange);
  addr_type map_name_addr =
      mem.getCallAddress((void *)(map_name_calls[0] + 0x9));
  // example: "maps/de_dust2.bsp"
  addr_type map_name_offset =
      map_name_addr - engineRange.first + 0x5; // add 5 because of "maps/"
  sprintf(offset_buf, "0x%lx", map_name_offset);
  offset_names.push_back("map_name_offset");
  offsets.push_back(string(offset_buf));

  cout << "-- Client State --" << endl;
  addr_type clientState_addr =
      mem.find_pattern(client_state_pattern, engineRange)[0];
  addr_type clientState_offset = clientState_addr - engineRange.first;
  sprintf(offset_buf, "0x%lx", clientState_offset);
  offset_names.push_back("clientState_offset");
  offsets.push_back(string(offset_buf));

  // netvar dumping
  cout << "-- dwGetAllClasses --" << endl;
  addr_type allClasses_call =
      mem.find_pattern(dwGetAllClasses_pattern, clientRange)[0];
  addr_type allClasses_addr =
      mem.getCallAddress((void *)(allClasses_call + 0x2));
  mem.read((void *)(allClasses_addr), (void *)&allClasses_addr,
           sizeof(allClasses_addr));
  mem.read((void *)(allClasses_addr), (void *)&allClasses_addr,
           sizeof(allClasses_addr));
  NetvarFinder netFinder(mem, allClasses_addr);
  netFinder.dump();
  std::map<std::string, std::uint64_t> const &netvars = netFinder.getNetvars();
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
  // "0x%lx",isConnected_offset); offset_names.push_back("isConnected_offset");
  // offsets.push_back(string(offset_buf));

  // cout << "-- SplitScreen --" << endl;
  // addr_type split_call = mem.find_pattern(isConnectedMove_pattern,
  // engineRange); addr_type isConnected_addr = mem.getCallAddress((void*)
  // (isConnected_call + 0x8)) + 1; addr_type isConnected_offset =
  // isConnected_addr - engineRange.first; sprintf(offset_buf,
  // "0x%lx",isConnected_offset); offset_names.push_back("isConnected_offset");
  // offsets.push_back(string(offset_buf));

  // cout << "Test1: " << test1 << endl;
  // cout << "Test2: " << test2 << endl;
  write_offsets(offset_names, offsets, file_name);
  print_offsets(offset_names, offsets);

  if (retain_settings && settings != "") {
    // rewrite settings
    ofstream file(file_name, ios_base::app);
    file.write(settings.c_str(), settings.size());
    file.close();
  } else {
    write_settings(file_name);
  }

  write_netvars(netvars, file_name);

  return 0;
}
