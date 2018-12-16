#pragma once

#include "misc/memory_access.hpp"
// based on https://github.com/Teklad/tuxdump/blob/master/src/tools/netvars.cpp
class ClientClass {
public:
  uintptr_t m_pCreateFn;
  uintptr_t m_pCreateEventF;
  uintptr_t m_pNetworkName;
  uintptr_t m_pRecvTable;
  uintptr_t m_pNext;
  int m_ClassID;
};

class NetvarFinder {
private:
  ClientClass cc;
  void walkTable(addr_type m_pRecvTable, int level);
  MemoryAccess& mem;
public:
  NetvarFinder(MemoryAccess& mem, addr_type);
  void dump();
};
