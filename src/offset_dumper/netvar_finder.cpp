#include <stdexcept>
#include "netvar_finder.hpp"

using namespace std;

NetvarFinder::NetvarFinder(MemoryAccess& mem, addr_type class_head) : mem(mem) {
  cc.m_pNext = class_head;
}

void NetvarFinder::dump() {
  if(cc.m_pNext == 0)
    throw runtime_error("Invalid class_head");
  while(cc.m_pNext) {
    mem.read((void*) cc.m_pNext, &cc, sizeof(cc));
    char tableName[64];
    if (cc.m_pRecvTable) {
      mem.read((void*) cc.m_pNetworkName, (void*) tableName, sizeof(tableName));
      cout << tableName << endl;
      // walkTable(cc.m_pRecvTable, 0);
    } else {
      break;
    }
  }
}

void NetvarFinder::walkTable(addr_type m_pRecvTable, int level) {
  char propName[64];
}
