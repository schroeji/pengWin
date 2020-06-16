#include "netvar_finder.hpp"

#include <stdexcept>
#include <map>


using namespace std;

NetvarFinder::NetvarFinder(MemoryAccess& mem, addr_type class_head) : mem(mem) {
  cc.m_pNext = class_head;
}

void NetvarFinder::dump() {
  if(cc.m_pNext == 0)
    throw runtime_error("Invalid class_head");
  while(cc.m_pNext) {
    mem.read((void*) cc.m_pNext, &cc, sizeof(cc));
    char networkName[64];
      if (cc.m_pRecvTable) {
        mem.read((void*) cc.m_pNetworkName, (void*) networkName, sizeof(networkName));
        // cout << networkName << endl;
        walkTable(cc.m_pRecvTable, 0, networkName);
      } else {
        break;
      }
    }
}

void NetvarFinder::walkTable(addr_type m_pRecvTable, int level, std::string prefix) {
  char tableName[64];
  char propName[64];
  RecvTable table;
  RecvProp prop;
  mem.read((void*) m_pRecvTable, (void*) &table, sizeof(table));
  mem.read((void*) table.m_pNetTableName, (void*) &tableName, sizeof(tableName));
  for (int i = 0; i < table.m_nProps; i++) {
    mem.read((void*) table.m_pProps + i * sizeof(prop), (void*) &prop, sizeof(prop));
    mem.read((void*) prop.m_pVarName, (void*) &propName, sizeof(propName));

    if (!strcmp(propName, "baseclass"))
      continue;

    if (isdigit(propName[0]) || prop.m_RecvType == SendProp::DPT_Array)
      continue;

    netvar_map.insert(std::make_pair(prefix + "::" + tableName + "::" + propName, prop.m_Offset));

    if (prop.m_RecvType == SendProp::DPT_DataTable && prop.m_pDataTable) {
      walkTable(prop.m_pDataTable, level + 1, prefix + "::" + tableName);
    }
  }
}


std::map<std::string, std::uint64_t> const& NetvarFinder::getNetvars() const {
  return netvar_map;
}
