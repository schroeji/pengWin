#include "memory_access.hpp"
#include "typedef.hpp"

#include <iostream>
#include <string>
#include <sys/uio.h>

using namespace std;

void MemoryAccess::updateAddrs() {
  local_player_addr_location = client_base + local_player_offset;
  // cout << hex << "local_player_addr: " << local_player_addr << endl;
  if(!read((void*) local_player_addr_location, &local_player_addr, sizeof(local_player_addr)))
    cout << "WARNING: could not get local_player" << endl;
  // cout << "local_player: " << local_player_addr << endl;

  glow_addr = client_base + glow_offset;
  cout << hex << "glow_addr: " << glow_addr << endl;

  attack_addr_call_location = client_base + attack_offset;
  // cout << hex << "attack_addr_call_location: " << attack_addr_call_location << endl;
  attack_addr = getCallAddress((void*) attack_addr_call_location);
  if(!attack_addr)
    cout << "WARNING: could not get attack_addr" << endl;
  // cout << hex << "attack_addr: " << attack_addr << endl;

  map_name_addr = engine_base + map_name_offset;
}

pid_t MemoryAccess::getPid() {
  FILE* in;
  char buf[128];
  string cmd = "pidof -s " + GAME_NAME;
  in = popen(cmd.c_str(), "r");
  if ( !(in && fgets(buf, 128, in)) )
    cout << "No PID found"<< endl;
  pclose(in);
  pid = strtoul(buf, NULL, 10);
  return pid;
}

unsigned long int MemoryAccess::getClientBase() {
  client_base = getModule("client_client.so");
  cout << hex << "Client Base: " << client_base << endl;
  updateAddrs();
  return client_base;
}

unsigned long int MemoryAccess::getEngineBase() {
  engine_base = getModule("engine_client.so");
  cout << hex << "Engine Base: " << engine_base << endl;
  updateAddrs();
  return engine_base;
}

unsigned long int MemoryAccess::getModule(const string& modname) {
  string cmd = "grep " + modname + " /proc/" + to_string(pid) + "/maps | head -n 1 | cut -d \"-\" -f1";
  FILE* in;
  char buf[128];
  in = popen(cmd.c_str(), "r");
  if ( !(in && fgets(buf, 128, in)))
    cout << "Could not open module: " << modname << endl;
  pclose(in);
  return strtoul(buf, NULL, 16);
}

bool MemoryAccess::read(void* addr, void* buff, size_t size) {
  iovec local_mem;
  iovec remote_mem;
  local_mem.iov_base = buff;
  local_mem.iov_len = size;
  remote_mem.iov_base = addr;
  remote_mem.iov_len = size;
  return (process_vm_readv(pid, &local_mem, 1, &remote_mem, 1, 0) == (signed) size);
}

bool MemoryAccess::write(void* addr, void* buff, size_t size) {
  iovec local_mem;
  iovec remote_mem;
  local_mem.iov_base = buff;
  local_mem.iov_len = size;
  remote_mem.iov_base = addr;
  remote_mem.iov_len = size;
  return (process_vm_writev(pid, &local_mem, 1, &remote_mem, 1, 0) == (signed) size);
}

unsigned long int MemoryAccess::getCallAddress(void* addr) {
  unsigned int jump_len;

  if (read((char*) addr+ 1, &jump_len, sizeof(unsigned int))) {
    return jump_len+ (unsigned long) addr+ 5;
  }

  return 0;
}


unsigned int MemoryAccess::getCrosshairTarget() {
  unsigned int ret;
  if(!read((void*) (local_player_addr + crosshair_id_offset), &ret, sizeof(ret)))
    cout << "WARNING: could not get crosshairTarget" << endl;
  return ret;
}

Team MemoryAccess::getTeam() {
  unsigned int team;
  if(!read((void*) (local_player_addr + team_number_offset), &team, sizeof(team)))
    cout << "WARNING: could not get Team" << endl;
  return (Team) team;
}

string MemoryAccess::getMapName() {
  char MapName[32];
  read((void*)(map_name_addr), &MapName, sizeof(MapName));
  // mem.read((void*)(Address + OFFSET_MAPNAME), &MapName, sizeof(MapName));
  string map_path(MapName);
  // vector<string> no_path = split_string(map_path, "/");
  vector<string> no_bsp = split_string(map_path, ".");
  string ret(no_bsp[0]);
  return ret;
}
