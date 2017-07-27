#include "memory_access.hpp"
#include "typedef.hpp"

#include <iostream>
#include <string>
#include <sys/uio.h>

using namespace std;

void MemoryAccess::updateAddrs() {
  local_player_addr = client_base + local_player_offset;
  cout << hex << "local_player_addr: " << local_player_addr << endl;
  read((void*) local_player_addr, &local_player, sizeof(local_player));
  cout << "local_player: " << local_player << endl;
  glow_addr = client_base + glow_offset;
  cout << hex << "glow_addr: " << glow_addr << endl;
}

pid_t MemoryAccess::getPid() {
  FILE* in;
  char buf[128];
  string cmd = "pidof -s " + GAME_NAME;
  // const char* cmd = ("pidof -s " + GAME_NAME).c_str();
  in = popen(cmd.c_str(), "r");
  if ( !(in && fgets(buf, 128, in)) )
    cout << "No PID found"<< endl;
  pclose(in);
  pid = strtoul(buf, NULL, 10);
  return pid;
}

uint64_t MemoryAccess::getClientBase() {
  client_base = getModule("client_client.so");
  cout << hex << "Client Base: " << client_base  << endl;
  updateAddrs();
  return client_base;
}

uint64_t MemoryAccess::getModule(const string& modname) {
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
