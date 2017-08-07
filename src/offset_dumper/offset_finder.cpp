#include "../memory_access.hpp"
#include "../typedef.hpp"

#include <iostream>

using namespace std;
int main(int argc, char** argv) {
  MemoryAccess mem;
  mem.getPid();
  Addr_Range clientRange = mem.getClientRange();
  Addr_Range engineRange = mem.getEngineRange();

  const char glowPointerCall_data[] = "\xE8\x00\x00\x00\x00\x48\x8b\x3d\x00\x00\x00\x00\xBE\x01\x00\x00\x00\xC7";
  const char glowPointerCall_pattern[] = "x????xxx????xxxxxx";
  const char local_player_addr_data[] = "\x48\x89\xe5\x74\x0e\x48\x8d\x05\x00\x00\x00\x00";
  const char local_player_addr_pattern[] = "xxxxxxxx????";
  const char atk_mov_data[] = "\x44\x89\xe8\x83\xe0\x01\xf7\xd8\x83\xe8\x03\x45\x84\xe4\x74\x00\x21\xd0";
  const char atk_mov_pattern[] = "xxxxxxxxxxxxxxx?xx";

  addr_type glowPointerCall = mem.find_pattern(glowPointerCall_data, glowPointerCall_pattern, clientRange);
  addr_type glowFunctionCall = mem.getCallAddress((void*) glowPointerCall);
  unsigned int glowOffset;
	mem.read((void*) (glowFunctionCall + 0x10), &glowOffset, sizeof(int));
  addr_type glowManAddr = glowFunctionCall + 0x10 + glowOffset + 0x4;
  addr_type glow_offset = glowManAddr - clientRange.first;

  addr_type localPlayerFunction = mem.find_pattern(local_player_addr_data, local_player_addr_pattern, clientRange);
  addr_type local_player_addr = mem.getCallAddress((void*) (localPlayerFunction + 0x7));
  addr_type local_player_offset = local_player_addr - clientRange.first;

  addr_type attack_addr = mem.find_pattern(atk_mov_data, atk_mov_pattern, clientRange);
  addr_type atk_offset = attack_addr - clientRange.first;
  cout << hex << glow_offset << endl;
  cout << hex <<  local_player_offset << endl;
  cout << atk_offset << endl;
}
