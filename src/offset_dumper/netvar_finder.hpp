#pragma once

#include <cstring>
#include "misc/memory_access.hpp"
// based on https://github.com/Teklad/tuxdump/blob/master/src/tools/netvars.cpp
// and https://github.com/ericek111/java-csgo-externals/blob/wip/src/me/lixko/csgoexternals/offsets/NetvarDumper.java
enum class SendProp: int {
                          DPT_Int = 0,
                          DPT_Float,
                          DPT_Vector,
                          DPT_VectorXY,
                          DPT_String,
                          DPT_Array,
                          DPT_DataTable,
                          DPT_Int64,
                          DPT_NUMSendPropTypes
};
class ClientClass {
public:
  uintptr_t m_pCreateFn;
  uintptr_t m_pCreateEventF;
  uintptr_t m_pNetworkName;
  uintptr_t m_pRecvTable;
  uintptr_t m_pNext;
  int m_ClassID;
};

class RecvProp {
public:
  uintptr_t m_pVarName; // char *
  SendProp m_RecvType;
  int m_Flags;
  int m_StringBufferSize;
  bool m_bInsideArray;
  char pad0[3];
  uintptr_t m_pExtraData; // const void *
  uintptr_t m_pArrayProp; // RecvProp *
  uintptr_t m_ArrayLengthProxy; //  (*ArrayLengthRecvProxyFn)( void *pStruct, int objectID, int currentArrayLength )
  uintptr_t m_ProxyFn; // (*RecvVarProxyFn)( const CRecvProxyData *pData, void *pStruct, void *pOut )
  uintptr_t m_DataTableProxyFn; // (*DataTableRecvVarProxyFn)(const RecvProp *pProp, void **pOut, void *pData, int objectID)
  uintptr_t m_pDataTable; // RecvTable *
  unsigned int m_Offset;
  int m_ElementStride;
  unsigned int m_nElements;
  char pad1[4];
  uintptr_t m_pParentArrayPropName; // const char *
	// 0x59
	// 0x3C on 32b
};

class RecvTable {
public:
  uintptr_t m_pProps; // RecvProp*
  int m_nProps;
  int pad0;
  uintptr_t m_pDecoder; // void*
  uintptr_t m_pNetTableName; // char*
  bool m_bInitialized;
  bool m_bInMainList;
};

class NetvarFinder {
private:
  ClientClass cc;
  void walkTable(addr_type m_pRecvTable, int level, std::string prefix);
  MemoryAccess& mem;
  std::map<std::string, std::uint64_t> netvar_map;
public:
  NetvarFinder(MemoryAccess& mem, addr_type);
  void dump();
  std::map<std::string, std::uint64_t> const& getNetvars() const;
};
