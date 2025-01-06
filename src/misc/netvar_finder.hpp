#pragma once
#include "memory_access.hpp"
#include "util.hpp"
#include <map>

class NetvarFinder {
private:
  MemoryAccess &mem_;
  std::map<std::string, addr_type> netvar_map_;
  void insert(std::string name, std::uint8_t *const addr, bool network_enable);

public:
  NetvarFinder(MemoryAccess &mem) : mem_{mem} {};
  void dump();
  addr_type getNetvar(std::string const &name) const;
  std::map<std::string, std::uint64_t> const &getNetvars() const;
  void printNetvars();
};
