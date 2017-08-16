#pragma once

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>

struct Vector {
	float x;
	float y;
	float z;
};

struct Color {
	unsigned char _color[4];
};

struct Vector2D {
	float x, y;
};

struct QAngle {
	float x; // Pitch
	float y; // Yaw
	float z; // Roll
};

struct EntityType {
	char __buf_0x00[0x88]; // 0x0
	int m_bIsAutoaimTarget; // 0x88
  char __buf_0x8c[0x8]; // 0x8C
  unsigned int m_iEntityId; //0x94
	// unsigned int m_iEntityId; // 0x8C
	// char __buf_0x8C[0x10]; // 0x90
  char __buf_0x98[0x8]; //0x98
	int m_clrRender; // 0xA0
	int m_cellbits; // 0xA4
	char __buf_0xA8[0x4]; // 0xA8
	int m_cellX; // 0xAC
	int m_cellY; // 0xB0
	int m_cellZ; // 0xB4
	char __buf_0xB8[0xC]; // 0xB8
	Vector m_vecAbsVelocity; // 0xC4
	Vector m_vecAbsOrigin; // 0xD0
	Vector m_vecOrigin; // 0xDC
	Vector m_vecAngVelocity; // 0xE8
	QAngle m_angAbsRotation; // 0xF4
	QAngle m_angRotation; // 0x100
	float m_flGravity; // 0x10C
	float m_flProxyRandomValue; // 0x110
	int m_iEFlags; // 0x114
	char __buf_0x11A[0x4];
	unsigned char m_nWaterType; // 0x118
	unsigned char m_bDormant; // 0x119
	char __buf_0x11ssA[0xA]; // 0x11A
	float m_fEffects; // 0x11C
	int m_iTeamNum; // 0x120
	int m_iPendingTeamNum; // 0x124
	int m_iHealth; // 0x12C
	unsigned long m_fFlags; // 0x130
	char __buf_0x130[0x8]; // 0x134
	// Vector m_vecViewOffset; // 0x134
	Vector m_vecVelocity; // 0x140
	Vector m_vecBaseVelocity; // 0x14C
	QAngle m_angNetworkAngles; // 0x158
	Vector m_vecNetworkOrigin; // 0x164
};

template<class T> class CUtlVector {
public:
  T* DataPtr; // 0000 (054612C0)
  unsigned int Max; // 0004 (054612C4)
  unsigned int unk02; // 0008 (054612C8)
  unsigned int Count; // 000C (054612CC)
  unsigned int DataPtrBack; // 0010 (054612D0)
};

struct ObjectType {
  void* m_pEntity;
  float m_flGlowRed;
  float m_flGlowGreen;
  float m_flGlowBlue;
  float m_flGlowAlpha;
  uint8_t unk0[16];
  bool m_bRenderWhenOccluded : 8;
  bool m_bRenderWhenUnoccluded : 8;
  bool m_bFullBloomRender : 8;
  unsigned char unk1;
  int m_nFullBloomStencilTestValue;
  int m_nSplitScreenSlot;
  int m_nNextFreeSlot;
  unsigned char unk2[4];

  static const int END_OF_FREE_LIST = -1;
  static const int ENTRY_IN_USE = -2;
}; // sizeof() == 0x34

enum Team {
  CT = 3,
  T = 2
};

class ObjectManager
{
public:
  CUtlVector<ObjectType> objects; // 0000
  int m_nFirstFreeSlot; // 0014 (054612D4)
  unsigned int unk1; // 0018 (054612D8)
  unsigned int unk2; // 001C (054612DC)
  unsigned int unk3; // 0020 (054612E0)
  unsigned int unk4; // 0024 (054612E4)
  unsigned int unk5; // 0028 (054612E8)
};

typedef std::pair<unsigned long int, unsigned long int> Addr_Range;
typedef unsigned long int addr_type;

std::vector<std::string> split_string(const std::string&, const std::string&);

void normalize_vector(Vector*);
Vector getDist(Vector*, Vector*);
float scalar_prod(Vector*, Vector*);

inline float degree_to_radian(float x) {
  return x*0.01745329;
}
inline float radian_to_degree(float x) {
  return x/0.01745329;
}
