#include "../memory_access.hpp"
#include "../typedef.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <dlfcn.h>

using namespace std;


void write_offsets(vector<string> names, vector<string> offsets, const string& file_name) {
  if(names.size() != offsets.size()){
    cout << "offset and name length not matching" << endl;
  }
  ofstream file(file_name);
  file << "# offsets" << endl;
  for (size_t i = 0; i < names.size(); i++){
    file << names[i] << "=" << offsets[i] << endl;
  }
  file.close();
}

void write_settings(const string& file_name) {
  ofstream file(file_name, ios_base::app);
  file << endl;
  file << "# settings" << endl;
  file << "main_loop_sleep=10" << endl;
  file << "trigger_delay=10" << endl;
  file << "trigger_use_random=false" << endl;
  file << "# find your key with \"$ dumpkeys -l\"" << endl;
  file << "trigger_key=Caps_Lock" << endl;
  file << "find_map=false" << endl;
  file << "debug=false" << endl;
  file.close();
}

int main(int argc, char** argv) {
  MemoryAccess mem(nullptr);
  mem.getPid();
  Addr_Range clientRange = mem.getClientRange();
  Addr_Range engineRange = mem.getEngineRange();
  cout << dec << "Client size: " << clientRange.second - clientRange.first << endl;
  cout << "Engine size: " << engineRange.second - engineRange.first << endl;
  const char glowPointerCall_data[] = "\xE8\x00\x00\x00\x00\x48\x8b\x3d\x00\x00\x00\x00\xBE\x01\x00\x00\x00\xC7";
  const char glowPointerCall_pattern[] = "x????xxx????xxxxxx";
  const char local_player_addr_data[] = "\x48\x89\xe5\x74\x0e\x48\x8d\x05\x00\x00\x00\x00";
  const char local_player_addr_pattern[] = "xxxxxxxx????";
  const char atk_mov_data[] = "\x44\x89\xe8\x83\xe0\x01\xf7\xd8\x83\xe8\x03\x45\x84\xe4\x74\x00\x21\xd0";
  const char atk_mov_pattern[] = "xxxxxxxxxxxxxxx?xx";

  const char clientState_data[] = "\xA1\x00\x00\x00\x00\x33\xD2\x6A\x00\x6A\x00\x33\xC9\x89\xB0";
  const char clientState_pattern[] = "x????xxxxxxxxxx";


  addr_type clientState_test = mem.find_pattern(clientState_data, clientState_pattern, engineRange);
  // addr_type viewAngels = mem.find_pattern(viewAngles_data, viewAngles_pattern, engineRange);
  // addr_type test = mem.find_pattern(glowObjManTest_data, glowObjManTest_pattern, clientRange);
  unsigned int test1, test2;
  const char local_player_test[] = "\xA3\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x59\xC3\x6A\x00";
  const char local_player_test_pattern[] = "x????xx????????x????xxx?";
  test1 = mem.find_pattern(local_player_test, local_player_test_pattern, clientRange);
  // mem.read((void*) test, &test1, sizeof(test1));
  // mem.read((void*) (test + 0x1), &test2, sizeof(test2));

  vector<string> offsets;
  vector<string> offset_names;
  char offset_buf[64];

  addr_type glowPointerCall = mem.find_pattern(glowPointerCall_data, glowPointerCall_pattern, clientRange);
  cout << hex << "GlowPointerCall at: " << glowPointerCall << endl;
  addr_type glowFunctionCall = mem.getCallAddress((void*) glowPointerCall);
  unsigned int glowOffset;
	mem.read((void*) (glowFunctionCall + 0x10), &glowOffset, sizeof(int));
  addr_type glowManAddr = glowFunctionCall + 0x10 + glowOffset + 0x4;
  addr_type glow_offset = glowManAddr - clientRange.first;
  sprintf(offset_buf, "0x%lx", glow_offset);
  offset_names.push_back("glow_offset");
  offsets.push_back(string(offset_buf));

  addr_type localPlayerFunction = mem.find_pattern(local_player_addr_data, local_player_addr_pattern, clientRange);
  cout << hex << "LocalPlayerFunctionCall at: " << localPlayerFunction << endl;
  addr_type local_player_addr = mem.getCallAddress((void*) (localPlayerFunction + 0x7));
  addr_type local_player_offset = local_player_addr - clientRange.first;
  sprintf(offset_buf, "0x%lx", local_player_offset);
  offset_names.push_back("local_player_offset");
  offsets.push_back(string(offset_buf));

  addr_type attack_addr = mem.find_pattern(atk_mov_data, atk_mov_pattern, clientRange);
  cout << hex << "attack_adr at: " << attack_addr << endl;
  addr_type atk_offset = attack_addr - clientRange.first;
  sprintf(offset_buf, "0x%lx", atk_offset);
  offset_names.push_back("attack_offset");
  offsets.push_back(string(offset_buf));

  cout << hex << "GlowMan: " << glowManAddr << "  offset: " << glow_offset << endl;
  cout << glowPointerCall << endl << glowFunctionCall << endl;
  cout << "clientStateTest: " << clientState_test << endl;
  cout << "test1: " << test1 << endl;
  // cout << "Test1: " << test1 << endl;
  // cout << "Test2: " << test2 << endl;
  // cout << viewAngels << endl;
  write_offsets(offset_names, offsets, "settings.cfg");
  write_settings("settings.cfg");
}
