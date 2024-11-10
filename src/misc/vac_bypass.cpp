#include "vac_bypass.h"
#include "util.hpp"
#include <cstdint>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

void VacBypass::set_inotify_max() {
  std::string cmd = "sysctl -w fs.inotify.max_user_instances=0";
  std::system(cmd.c_str());
}

addr_type VacBypass::filter_address(pid_t pid, addr_type addr) {
  // Construct the path to the pagemap file
  std::string pagemapPath = "/proc/" + std::to_string(pid) + "/pagemap";
  unsigned long pageNum = addr / kPageSize;
  unsigned long pageSize = sizeof(std::uint64_t);

  int fd = ::open(pagemapPath.c_str(), O_RDONLY);
  if (fd < 0) {
    perror("Failed to open pagemap file");
    return 0;
  }

  off_t offset = pageNum * pageSize;
  if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
    perror("Failed to seek in pagemap file");
    close(fd);
    return 0;
  }

  uint64_t entry;
  ssize_t bytes_read = ::read(fd, &entry, pageSize);
  if (bytes_read != pageSize) {
    close(fd);
    return 0;
  }

  close(fd);

  // // Open the pagemap file
  // std::ifstream pagemapFile(pagemapPath, std::ios::binary);
  // if (!pagemapFile) {
  //   perror("Error opening pagemap");
  //   return 0;
  // }

  // // Calculate the page number and offset
  // unsigned long pageNum = addr / kPageSize;
  // std::cout << "Page Num: " << pageNum << std::endl;
  // pagemapFile.seekg(pageNum * sizeof(uint64_t));

  // // Read the entry
  // uint64_t entry;
  // pagemapFile.read(reinterpret_cast<char *>(&entry), sizeof(entry));
  // if (!pagemapFile) {
  //   perror("Error reading pagemap");
  //   return 0;
  // }

  bool const valid{(entry & (1ULL << 63)) ? true
                                          : false}; // 1 if resident, 0 if not
  if (valid) {
    return addr;
  }
  // std::cout << "TRAP Address detected: " << addr << std::endl;
  return 0;
}
