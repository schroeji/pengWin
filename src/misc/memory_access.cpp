#include "memory_access.hpp"
#include "settings.hpp"
#include "typedef.hpp"
#include "util.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/uio.h>
#include <vector>

using namespace std;

MemoryAccess::MemoryAccess(Settings *settings) : settings(settings) {
  if (settings->use_kernel_module) {
    kernel_module_file = fopen(kernel_module_file_path.c_str(), "rb");
    if (!kernel_module_file) {
      cout << "Failed to open " << kernel_module_file_path << std::endl;
      throw("Could not open kernel_module_file");
    }
    cout << "Successfully opened " << kernel_module_file_path << " for reading."
         << std::endl;
  }
  vac_bypass.set_inotify_max();
}
MemoryAccess::~MemoryAccess() {
  if (settings->use_kernel_module) {
    fclose(kernel_module_file);
  }
}

unsigned int MemoryAccess::read_offset(void *addr) { return read_uint32(addr); }

unsigned int MemoryAccess::read_uint32(void *addr) {
  std::uint32_t value;
  read(addr, &value, sizeof(std::uint32_t));
  return value;
}

std::uint8_t MemoryAccess::read_uint8(void *addr) {
  std::uint8_t value;
  read(addr, &value, sizeof(std::uint8_t));
  return value;
}

addr_type MemoryAccess::get_address(void *pointer) {
  addr_type pointee;
  read(pointer, &pointee, sizeof(pointee));
  return pointee;
}

void MemoryAccess::updateAddrs() {
  // local_player_addr_location = client_range.first + local_player_offset;
  // if (!read((void *)local_player_addr_location, &local_player_addr,
  //           sizeof(local_player_addr)))
  //   if (settings != nullptr && settings->debug)
  //     cout << "WARNING: could not get local_player" << endl;

  // glow_addr = client_range.first + glow_offset;
  // if (debug)
  //   cout << hex << "glow_addr: " << glow_addr << endl;

  // attack_addr_call_location = client_range.first + attack_offset;
  // if (settings != nullptr) {
  //   attack_addr = getCallAddress((void *)attack_addr_call_location);
  //   if (!attack_addr)
  //     if (settings->debug)
  //       cout << "WARNING: could not get attack_addr" << endl;
  // }

  // force_jump_addr = client_range.first + force_jump_offset;

  // map_name_addr = engine_range.first + map_name_offset;
  // isConnected_addr = engine_range.first + isConnected_offset;
  // clientState_addr = engine_range.first + clientState_offset;
}

pid_t MemoryAccess::getPid() {
  FILE *in;
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

void MemoryAccess::printAddrRangeVec(std::vector<Addr_Range> const &range_vec) {
  for (auto range : range_vec) {
    cout << range.first << " - " << range.second << endl;
  }
}

std::vector<Addr_Range> &MemoryAccess::getClientRange() {
  if (client_range.empty()) {
    client_range = getModule("libclient.so");
    cout << hex << "Client Range: " << endl;
    printAddrRangeVec(client_range);
  }
  return client_range;
}

std::vector<Addr_Range> MemoryAccess::getPanoramaClientRange() {
  panorama_client_range = getModule("libpanoramauiclient.so");
  cout << hex << "Panorama Client Range: " << endl;
  printAddrRangeVec(panorama_client_range);
  updateAddrs();
  return panorama_client_range;
}

std::vector<Addr_Range> MemoryAccess::getEngineRange() {
  engine_range = getModule("libengine2.so");
  cout << hex << "Engine Range: " << endl;
  printAddrRangeVec(engine_range);
  updateAddrs();
  return engine_range;
}

std::vector<Addr_Range>
MemoryAccess::getModuleFromKernelModule(const string &modname) {
  std::vector<Addr_Range> result{};
  static constexpr std::size_t kMapsBufferSize{0x20000};
  // Magic number that is handled by the kernel module to get the memory
  // mappings of the file.
  static constexpr off64_t kMapsMagicNumber{0xFFFF};
  char maps_buffer[kMapsBufferSize];
  std::size_t read_sum = pread(fileno(kernel_module_file), maps_buffer,
                               kMapsBufferSize, kMapsMagicNumber);
  std::cout << "Read " << read_sum << " bytes from kernel module maps."
            << std::endl;
  std::string maps_string(maps_buffer, &maps_buffer[read_sum]);
  std::istringstream maps_stream(maps_string);
  std::string line;
  std::cout << "Memory mappings for library: " << modname << std::endl;
  while (std::getline(maps_stream, line)) {
    if (line.find(modname) != std::string::npos) {
      std::istringstream iss(line);
      std::string addressRange, perms, offset, dev, inode, pathname;

      // Read the line into respective fields
      iss >> addressRange >> perms >> offset >> dev >> inode;
      std::getline(iss, pathname);
      auto split = split_string(addressRange, "-");
      result.push_back(Addr_Range(strtoul(split[0].c_str(), NULL, 16),
                                  strtoul(split[1].c_str(), NULL, 16)));
      // Output the parsed information
      // std::cout << "Address Range: " << addressRange << std::endl;
      // std::cout << "Permissions: " << perms << std::endl;
      // std::cout << "Offset: " << offset << std::endl;
      // std::cout << "Device: " << dev << std::endl;
      // std::cout << "Inode: " << inode << std::endl;
      // std::cout << "Pathname: " << pathname << std::endl;
      // std::cout << "-----------------------------" << std::endl;
    }
  }
  return result;
}

std::vector<Addr_Range> MemoryAccess::getModule(const string &modname) {
  if (settings->use_kernel_module) {
    return getModuleFromKernelModule(modname);
  }
  std::string mapsFilePath = "/proc/" + std::to_string(pid) + "/maps";
  std::ifstream mapsFile(mapsFilePath);
  std::vector<Addr_Range> result{};
  if (!mapsFile.is_open()) {
    std::cerr << "Could not open " << mapsFilePath << std::endl;
    return result;
  }
  std::string line;
  std::cout << "Memory mappings for library: " << modname << std::endl;

  while (std::getline(mapsFile, line)) {
    if (line.find(modname) != std::string::npos) {
      std::istringstream iss(line);
      std::string addressRange, perms, offset, dev, inode, pathname;

      // Read the line into respective fields
      iss >> addressRange >> perms >> offset >> dev >> inode;
      std::getline(iss, pathname);
      auto split = split_string(addressRange, "-");
      result.push_back(Addr_Range(strtoul(split[0].c_str(), NULL, 16),
                                  strtoul(split[1].c_str(), NULL, 16)));
      // Output the parsed information
      // std::cout << "Address Range: " << addressRange << std::endl;
      // std::cout << "Permissions: " << perms << std::endl;
      // std::cout << "Offset: " << offset << std::endl;
      // std::cout << "Device: " << dev << std::endl;
      // std::cout << "Inode: " << inode << std::endl;
      // std::cout << "Pathname: " << pathname << std::endl;
      // std::cout << "-----------------------------" << std::endl;
    }
  }
  return result;
}

bool MemoryAccess::read(addr_type addr, void *buff, size_t size) {
  return read((void *)addr, buff, size);
}

bool MemoryAccess::read(void *addr, void *buff, size_t size) {
  void *filtered_addr = (void *)vac_bypass.filter_address(pid, (addr_type)addr);
  if (filtered_addr && settings->use_kernel_module) {
    int ret = fseeko64(kernel_module_file, (off64_t)addr, SEEK_SET);
    if (ret < 0) {
      cout << "Failed to seek to address: "
           << std::to_string((off64_t)filtered_addr) << std::endl;
      return false;
    }
    ret = fread(buff, 1, size, kernel_module_file);
    if (ret < 0) {
      cout << "Failed to read from kernel module file.";
      return false;
    }
    return true;
  } else if (filtered_addr) {
    iovec local_mem;
    iovec remote_mem;
    local_mem.iov_base = buff;
    local_mem.iov_len = size;
    remote_mem.iov_base = filtered_addr;
    remote_mem.iov_len = size;
    return (process_vm_readv(pid, &local_mem, 1, &remote_mem, 1, 0) ==
            (signed)size);
  } else {
    if (settings->debug)
      cout << "Invalid address encountered: " << addr << endl;
  }
  return false;
}

bool MemoryAccess::write(void *addr, void *buff, size_t size) {
  // iovec local_mem;
  // iovec remote_mem;
  // local_mem.iov_base = buff;
  // local_mem.iov_len = size;
  // remote_mem.iov_base = addr;
  // remote_mem.iov_len = size;
  // return (process_vm_writev(pid, &local_mem, 1, &remote_mem, 1, 0) ==
  //         (signed)size);
}
addr_type MemoryAccess::getCallAddress(void *addr) {
  unsigned int jump_len;

  if (read((char *)addr + 1, &jump_len, sizeof(unsigned int))) {
    return jump_len + (unsigned long)addr + 5;
  }

  return 0;
}

addr_type MemoryAccess::getAbsoluteAddress(void *addr, int offset, int size) {
  unsigned int jump_len;
  if (read((char *)addr + offset, &jump_len, sizeof(unsigned int))) {
    return jump_len + (unsigned long)addr + size + offset;
  }
  return 0;
}

void MemoryAccess::updateLocalPlayerAddr() {
  // if (!read((void *)local_player_addr_location, &local_player_addr,
  //           sizeof(local_player_addr)))
  //   if (settings->debug)
  //     cout << "WARNING: could not get localplayer" << endl;
}

BoneInfo *MemoryAccess::getBoneMatrix(addr_type player) {
  if (player == 0)
    throw runtime_error("getBoneMatrix: Player is nullptr.");
  addr_type boneMatrix_addr;
  if (!read((void *)(player + m_dwBoneMatrix), &boneMatrix_addr,
            sizeof(boneMatrix_addr)))
    throw runtime_error("Could not get BoneMatrix address.");
  BoneInfo *boneMatrix = new BoneInfo[MAX_BONES];
  if (!read((void *)boneMatrix_addr, boneMatrix, sizeof(BoneInfo) * MAX_BONES))
    throw runtime_error("Could not get BoneMatrix");
  return boneMatrix;
}

// Vector MemoryAccess::getBone(BoneInfo* boneMatrix, unsigned int boneID) {

//   BoneInfo bone;
//   if (!read((void*) (boneMatrix_addr + sizeof(BoneInfo) * boneID), &bone,
//   sizeof(bone)))
//     throw runtime_error("Could not get BoneInfo.");
//   // bone location vectors have a different order than m_vecOrigin
//   // printf("bone: %f, %f, %f\n", bone.y, bone.z, bone.x);
//   return {bone.y, bone.z, bone.x};
// }

void MemoryAccess::printBlock(addr_type addr, size_t size) {
  unsigned char buffer[size];
  read((void *)addr, buffer, size);
  for (size_t i = 0; i < size; i++) {
    if (i % 16 == 0)
      cout << hex << endl
           << addr + i << "[" << setw(2) << setfill('0') << i << "]"
           << ":";
    cout << hex << setw(2) << setfill('0') << (unsigned int)buffer[i] << " ";
  }
  cout << endl;
}

vector<int> MemoryAccess::diffMem(addr_type addr, size_t size) {
  vector<int> result;
  if (diffBuffer == nullptr) {
    diffBuffer = (unsigned char *)malloc(size);
    read((void *)addr, diffBuffer, size);
    return result;
  }
  unsigned char buffer[size];
  read((void *)addr, buffer, size);
  for (size_t i = 0; i < size; i++) {
    if (diffBuffer[i] != buffer[i]) {
      result.push_back(i);
    }
  }
  if (result.size() > 0)
    memcpy(diffBuffer, buffer, size);
  return result;
}
