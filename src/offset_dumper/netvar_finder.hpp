#pragma once

#include <cstdint>
#include <cstring>
#include <map>
#include <string>
// based on https://github.com/Teklad/tuxdump/blob/master/src/tools/netvars.cpp
// and
// https://github.com/ericek111/java-csgo-externals/blob/wip/src/me/lixko/csgoexternals/offsets/NetvarDumper.java
enum class SendProp : int {
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

using CreateClientClass_t = void *(*)(int entnum, int serialNum);
using CreateEvent_t = void *(*)();

class RecvTable {
public:
  uintptr_t m_pProps; // RecvProp*
  int m_nProps;
  int pad0;
  uintptr_t m_pDecoder;  // void*
  char *m_pNetTableName; // char*
  bool m_bInitialized;
  bool m_bInMainList;
};

class RecvProp {
public:
  uintptr_t m_pVarName; // char *
  SendProp m_RecvType;
  int m_Flags;
  int m_StringBufferSize;
  bool m_bInsideArray;
  char pad0[3];
  uintptr_t m_pExtraData;       // const void *
  uintptr_t m_pArrayProp;       // RecvProp *
  uintptr_t m_ArrayLengthProxy; //  (*ArrayLengthRecvProxyFn)( void *pStruct,
                                //  int objectID, int currentArrayLength )
  uintptr_t m_ProxyFn; // (*RecvVarProxyFn)( const CRecvProxyData *pData, void
                       // *pStruct, void *pOut )
  uintptr_t
      m_DataTableProxyFn;  // (*DataTableRecvVarProxyFn)(const RecvProp *pProp,
                           // void **pOut, void *pData, int objectID)
  RecvTable *m_pDataTable; // RecvTable *
  unsigned int m_Offset;
  int m_ElementStride;
  unsigned int m_nElements;
  char pad1[4];
  uintptr_t m_pParentArrayPropName; // const char *
                                    // 0x59
                                    // 0x3C on 32b
};

class ClientClass {
public:
  CreateClientClass_t m_pCreateFn;
  CreateEvent_t m_pCreateEventF;
  char *m_pNetworkName;
  RecvTable *m_pRecvTable;
  ClientClass *m_pNext;
  int m_ClassID;
};

struct i_baseclientdll {
  ClientClass *get_all_classes() {
    uintptr_t *vtable = *reinterpret_cast<uintptr_t **>(this);
    return reinterpret_cast<ClientClass *(*)(void *ecx)>(vtable[8])(this);
  }
};

class NetvarFinder {
private:
  ClientClass cc;
  void walkTable(RecvTable *m_pRecvTable, int level, std::string prefix);
  std::map<std::string, std::uint64_t> netvar_map;

public:
  NetvarFinder(ClientClass client_class);
  void dump();
  std::map<std::string, std::uint64_t> const &getNetvars() const;
};
