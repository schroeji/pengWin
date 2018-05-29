#pragma once

#include "util.hpp"
// definitions for the SDK
struct EntityType {
	char __buf_0x00[0x88]; // 0x0
	int m_bIsAutoaimTarget; // 0x88
  char __buf_0x8c[0x8]; // 0x8C
  unsigned int m_iEntityId; //0x94
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
	char __buf_0x118[0x4]; // 0x118
	unsigned char m_nWaterType; // 0x11C
	char __buf_0x11D[0x4]; // 0x11D
  unsigned char m_bDormant; // 0x121
  char __buf_0x122[6]; // 0x122
	float m_fEffects; // 0x128
	int m_iTeamNum; // 0x12C
	int m_iPendingTeamNum; // 0x130
	int m_iHealth; // 0x134
	unsigned long m_fFlags; // 0x138
	Vector2D m_vecViewOffset; // 0x13C
	Vector m_vecVelocity; // 0x148
	Vector m_vecBaseVelocity; // 0x154
	QAngle m_angNetworkAngles; // 0x160
	Vector m_vecNetworkOrigin; // 0x16C
};

template<class T> class CUtlVector {
public:
  T* DataPtr; // 0000 (054612C0)
  unsigned int Max; // 0004 (054612C4)
  unsigned int unk02; // 0008 (054612C8)
  unsigned int Count; // 000C (054612CC)
  unsigned int DataPtrBack; // 0010 (054612D0)
};

struct GlowObjectDefinition_t {
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

struct BoneInfo {
	char __pad0x0[0xA];
	float x;
	char __pad0x10[0xA];
	float y;
	char __pad0x20[0xA];
	float z;
};

class GlowObjectManager_t
{
public:
  CUtlVector<GlowObjectDefinition_t> objects; // 0000
  int m_nFirstFreeSlot; // 0014 (054612D4)
  unsigned int unk1; // 0018 (054612D8)
  unsigned int unk2; // 001C (054612DC)
  unsigned int unk3; // 0020 (054612E0)
  unsigned int unk4; // 0024 (054612E4)
  unsigned int unk5; // 0028 (054612E8)
};


enum Weapon {
  NONE = 0,
  DEAGLE = 1,
	ELITE = 2,
	FIVESEVEN = 3,
	GLOCK = 4,
	AK47 = 7,
	AUG = 8,
	AWP = 9,
	FAMAS = 10,
	G3SG1 = 11,
	GALILAR = 13,
	M249 = 14,
	M4A1 = 16,
	MAC10 = 17,
	P90 = 19,
	UMP45 = 24,
	XM1014 = 25,
	BIZON = 26,
	MAG7 = 27,
	NEGEV = 28,
	SAWEDOFF = 29,
	TEC9 = 30,
	TASER = 31,
	HKP2000 = 32,
	MP7 = 33,
	MP9 = 34,
	NOVA = 35,
	P250 = 36,
	SCAR20 = 38,
	SG556 = 39,
	SSG08 = 40,
	KNIFE = 42,
	FLASHBANG = 43,
	HEGRENADE = 44,
	SMOKEGRENADE = 45,
	MOLOTOV = 46,
	DECOY = 47,
	INCGRENADE = 48,
	C4 = 49,
	KNIFE_T = 59,
	M4A1_SILENCER = 60,
	USP_SILENCER = 61,
	CZ75A = 63,
	REVOLVER = 64,
	KNIFE_BAYONET = 500,
	KNIFE_FLIP = 505,
	KNIFE_GUT = 506,
	KNIFE_KARAMBIT = 507,
	KNIFE_M9_BAYONET = 508,
	KNIFE_TACTICAL = 509,
	KNIFE_FALCHION = 512,
	KNIFE_SURVIVAL_BOWIE = 514,
	KNIFE_BUTTERFLY = 515,
	KNIFE_PUSH = 516
};

// m_fFlags
// source: https://github.com/pmrowla/hl2sdk-csgo/blob/master/public/const.h#L110
#define	FL_ONGROUND				(1<<0)	// At rest / on the ground
#define FL_DUCKING				(1<<1)	// Player flag -- Player is fully crouched
#define	FL_WATERJUMP			(1<<3)	// player jumping out of water
#define FL_ONTRAIN				(1<<4) // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN				(1<<5)	// Indicates the entity is standing in rain
#define FL_FROZEN				(1<<6) // Player is frozen for 3rd person camera
#define FL_ATCONTROLS			(1<<7) // Player can't move, but keeps key inputs for controlling another entity
#define	FL_CLIENT				(1<<8)	// Is a player
#define FL_FAKECLIENT			(1<<9)	// Fake client, simulated server side; don't send network messages to them
#define	FL_INWATER				(1<<10)	// In water
// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define	FL_FLY					(1<<11)	// Changes the SV_Movestep() behavior to not need to be on ground
#define	FL_SWIM					(1<<12)	// Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
#define	FL_CONVEYOR				(1<<13)
#define	FL_NPC					(1<<14)
#define	FL_GODMODE				(1<<15)
#define	FL_NOTARGET				(1<<16)
#define	FL_AIMTARGET			(1<<17)	// set if the crosshair needs to aim onto the entity
#define	FL_PARTIALGROUND		(1<<18)	// not all corners are valid
#define FL_STATICPROP			(1<<19)	// Eetsa static prop!
#define FL_GRAPHED				(1<<20) // worldgraph has this ent listed as something that blocks a connection
#define FL_GRENADE				(1<<21)
#define FL_STEPMOVEMENT			(1<<22)	// Changes the SV_Movestep() behavior to not do any processing
#define FL_DONTTOUCH			(1<<23)	// Doesn't generate touch functions, generates Untouch() for anything it was touching when this flag was set
#define FL_BASEVELOCITY			(1<<24)	// Base velocity has been applied this frame (used to convert base velocity into momentum)
#define FL_WORLDBRUSH			(1<<25)	// Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
#define FL_OBJECT				(1<<26) // Terrible name. This is an object that NPCs should see. Missiles, for example.
#define FL_KILLME				(1<<27)	// This entity is marked for death -- will be freed by game DLL
#define FL_ONFIRE				(1<<28)	// You know...
#define FL_DISSOLVING			(1<<29) // We're dissolving!
#define FL_TRANSRAGDOLL			(1<<30) // In the process of turning into a client side ragdoll.
#define FL_UNBLOCKABLE_BY_PLAYER (1<<31)
