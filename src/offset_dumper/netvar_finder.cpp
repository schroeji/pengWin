#include "netvar_finder.hpp"

#include <iostream>
#include <map>
#include <stdexcept>

using namespace std;

NetvarFinder::NetvarFinder(ClientClass client_class) : cc(client_class) {}

void NetvarFinder::dump() {
  std::cout << "Dumping Netvars ..." << std::endl;
  if (cc.m_pNext == 0)
    throw runtime_error("Invalid class_head");
  while (cc.m_pNext) {
    printf("Next address %lu \n", cc.m_pNext);
    cc = *cc.m_pNext;
    printf("ASD \n");
    if (cc.m_pRecvTable) {
      std::string network_name{cc.m_pNetworkName};
      std::cout << "Dumping " << network_name << std::endl;
      walkTable(cc.m_pRecvTable, 0, network_name);
    } else {
      break;
    }
  }
  std::cout << "Dumping Netvars complete." << std::endl;
}

void NetvarFinder::walkTable(RecvTable *table, int level, std::string prefix) {
  char propName[64];
  for (int i = 0; i < table->m_nProps; i++) {
    std::string table_name(table->m_pNetTableName);

    std::cout << "Dumping " << table_name << std::endl;
    RecvProp *prop{
        reinterpret_cast<RecvProp *>(&table->m_nProps + i * sizeof(RecvProp))};
    if (!strcmp(propName, "baseclass"))
      continue;

    if (isdigit(propName[0]) || prop->m_RecvType == SendProp::DPT_Array)
      continue;

    netvar_map.insert(std::make_pair(
        prefix + "::" + table_name + "::" + propName, prop->m_Offset));

    if (prop->m_RecvType == SendProp::DPT_DataTable && prop->m_pDataTable) {
      walkTable(prop->m_pDataTable, level + 1, prefix + "::" + table_name);
    }
  }
}

std::map<std::string, std::uint64_t> const &NetvarFinder::getNetvars() const {
  return netvar_map;
}
