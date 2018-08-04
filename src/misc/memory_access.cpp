#include "memory_access.hpp"
#include "typedef.hpp"
#include "settings.hpp"
#include "util.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <string.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

MemoryAccess::MemoryAccess(Settings* settings) : settings(settings) {
  if (settings != nullptr) {
    glow_offset = settings->glow_offset;
    attack_offset = settings->attack_offset;
    local_player_offset = settings->local_player_offset;
    map_name_offset = settings->map_name_offset;
    force_jump_offset = settings->force_jump_offset;
    isConnected_offset = settings->isConnected_offset;
    debug = settings->debug;
  }
}

void MemoryAccess::updateAddrs() {
  local_player_addr_location = client_range.first + local_player_offset;
  if (!read((void*) local_player_addr_location, &local_player_addr, sizeof(local_player_addr)))
    if (settings != nullptr && settings->debug) cout << "WARNING: could not get local_player" << endl;

  glow_addr = client_range.first + glow_offset;
  if (debug) cout << hex << "glow_addr: " << glow_addr << endl;

  attack_addr_call_location = client_range.first + attack_offset;
  if (settings != nullptr) {
    attack_addr = getCallAddress((void*) attack_addr_call_location);
    if(!attack_addr)
      if (settings->debug) cout << "WARNING: could not get attack_addr" << endl;
  }

  force_jump_addr = client_range.first + force_jump_offset;
  map_name_addr = engine_range.first + map_name_offset;
  isConnected_addr = engine_range.first + isConnected_offset;
}

pid_t MemoryAccess::getPid() {
  FILE* in;
  char buf[128] = {0};
  string cmd = "pidof -s " + GAME_NAME;
  // when the pid has been found it might take some additional time
  // until all modules are laoded
  pid = 0;
  in = popen(cmd.c_str(), "r");
  fgets(buf, 128, in);
  pclose(in);
  pid = strtoul(buf, NULL, 10);
  return pid;
}

Addr_Range MemoryAccess::getClientRange() {
  client_range = getModule("client_client.so");
  cout << hex << "Client Base: " << client_range.first << endl;
  updateAddrs();
  return client_range;
}

Addr_Range MemoryAccess::getEngineRange() {
  engine_range = getModule("engine_client.so");
  cout << hex << "Engine Base: " << engine_range.first << endl;
  updateAddrs();
  return engine_range;
}

Addr_Range MemoryAccess::getModule(const string& modname) {
  string cmd = "grep " + modname + " /proc/" + to_string(pid) + "/maps | head -n 1";
  FILE* in;
  char buf[512];
  in = popen(cmd.c_str(), "r");
  if ( !(in && fgets(buf, sizeof(buf), in))) {
    cout << "WARNING: Could not open module: " << modname << endl;
    return Addr_Range(0, 0);
  }
  pclose(in);
  string line(buf);
  vector<string> splits = split_string(line, " ");
  vector<string> range = split_string(splits[0], "-");
  return Addr_Range(strtoul(range[0].c_str(), NULL, 16), strtoul(range[1].c_str(), NULL, 16));
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


addr_type MemoryAccess::find_pattern(const string& pattern, Addr_Range range) {
  size_t begin = range.first;
  size_t end = range.second;
  char buffer[4096];
  size_t len = pattern.size();
  size_t blocksize = sizeof(buffer);
  size_t totalsize = end - begin;
  size_t chunknum = 0;

  while (totalsize > 0) {
    size_t readsize = min(totalsize, blocksize);
    size_t readaddr = begin + (blocksize * chunknum);
    bzero(buffer, blocksize);
    if (read((void*) readaddr, buffer, readsize)) {
      for (size_t b = 0; b < readsize; b++) {
        // iterator for pattern string
        size_t i = 0;
        // number of matched bytes s
        size_t matches = 0;
        char byte = (char) strtol(pattern.substr(i, 2).c_str(), NULL, 16);
        while (buffer[b + matches] == byte || pattern.substr(i, 2) == "??") {
          matches++; // one matched byte
          i += 3; // skip over two characters plus space
          if (i >= len) {
            return (addr_type) (readaddr + b);
          }
          byte = (char) strtol(pattern.substr(i, 2).c_str(), NULL, 16);
        }
      }
    }
    totalsize -= readsize;
    chunknum++;
  }
  cout << "WARNING: no match for pattern" << endl;
  return 0x0;
}

addr_type MemoryAccess::getCallAddress(void* addr) {
  unsigned int jump_len;

  if (read((char*) addr + 1, &jump_len, sizeof(unsigned int))) {
    return jump_len + (unsigned long) addr + 5;
  }

  return 0;
}

addr_type MemoryAccess::getAbsoluteAddress(void* addr, int offset, int size) {
  unsigned int jump_len;
  if (read((char*) addr + offset, &jump_len, sizeof(unsigned int))) {
    return jump_len + (unsigned long) addr + size;
  }
  return 0;
}

void MemoryAccess::updateLocalPlayerAddr() {
  if (!read((void*) local_player_addr_location, &local_player_addr, sizeof(local_player_addr)))
    if (settings->debug) cout << "WARNING: could not get localplayer" << endl;
}

BoneInfo* MemoryAccess::getBoneMatrix(addr_type player) {
  if(player == 0)
    throw runtime_error("getBoneMatrix: Player is nullptr.");
  addr_type boneMatrix_addr;
  if (!read((void*) (player + m_dwBoneMatrix), &boneMatrix_addr, sizeof(boneMatrix_addr)))
    throw runtime_error("Could not get BoneMatrix address.");
  BoneInfo* boneMatrix = new BoneInfo[MAX_BONES];
  if (!read((void*) boneMatrix_addr, boneMatrix, sizeof(BoneInfo) * MAX_BONES))
    throw runtime_error("Could not get BoneMatrix");
  return boneMatrix;
}

// Vector MemoryAccess::getBone(BoneInfo* boneMatrix, unsigned int boneID) {

//   BoneInfo bone;
//   if (!read((void*) (boneMatrix_addr + sizeof(BoneInfo) * boneID), &bone, sizeof(bone)))
//     throw runtime_error("Could not get BoneInfo.");
//   // bone location vectors have a different order than m_vecOrigin
//   // printf("bone: %f, %f, %f\n", bone.y, bone.z, bone.x);
//   return {bone.y, bone.z, bone.x};
// }

void MemoryAccess::printBlock(addr_type addr, size_t size) {
  unsigned char buffer[size];
  read((void*) addr, buffer, size);
  for (size_t i = 0; i < size; i++) {
    if (i % 16 == 0)
      cout << hex << endl << addr + i << ":";
    cout << hex << setw(2) << setfill('0') << (unsigned int)buffer[i] << " ";
  }
  cout << endl;
}

vector<int> MemoryAccess::diffMem(addr_type addr, size_t size) {
  vector<int> result;
  if (diffBuffer == nullptr) {
    diffBuffer = (unsigned char*) malloc(size);
    read((void*) addr, diffBuffer, size);
    return result;
  }
  unsigned char buffer[size];
  read((void*) addr, buffer, size);
  for (size_t i = 0; i < size; i++) {
    if (diffBuffer[i] != buffer[i]) {
      result.push_back(i);
    }
  }
  if (result.size() > 0)
    memcpy(diffBuffer, buffer, size);
  return result;
}
