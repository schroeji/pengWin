#pragma once
#include "memory_access.hpp"

class Clicker {
private:
  MemoryAccess& mem;

public:
  Clicker(MemoryAccess& mem);
  // clicks using X window system
  void xClick();
  // clicks using memory write
  void memClick();
};
