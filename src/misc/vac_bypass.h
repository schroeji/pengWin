#ifndef VAC_BYPASS_H_
#define VAC_BYPASS_H_
#include "util.hpp"
#include <unistd.h> /* pread, sysconf */

class VacBypass {
public:
  VacBypass() = default;
  // Sets the inotify max limit to 0 so VAC can not use inotify to check if a
  // process is accessing /proc/$/maps
  void set_inotify_max();
  // Checks if a given address is mapped to physical memory to avoid reading
  // non-mapped data which could be detected by VAC using mincore syscalls
  addr_type filter_address(pid_t pid, addr_type);

private:
  long const kPageSize{sysconf(_SC_PAGE_SIZE)};
};

#endif // VAC_BYPASS_H_
