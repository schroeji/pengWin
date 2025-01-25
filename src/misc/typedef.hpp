#pragma once

#include "util.hpp"
#include <cstdint>

struct CEntityInstance {};

struct CEntityIdentity {
  CEntityInstance *entity;
  char __buf_0x08[8];
  std::uint32_t handle;
  char __buf_0x12[100];
};

static_assert(sizeof(CEntityIdentity) == 120);

struct CConcreteEntityList {
  static constexpr auto kNumberOfChunks{64};
  static constexpr auto kNumberOfIdentitiesPerChunk{512};

  using EntityChunk = CEntityIdentity[kNumberOfIdentitiesPerChunk];

  EntityChunk *chunks[kNumberOfChunks];
};

struct Player {
  Player(addr_type entity_addr, int health, Vector origin, Team team,
         bool is_defusing, Weapon weapon, QAngle networkAngle, QAngle aimPunch,
         Vector viewOrigin)
      : entity_addr(entity_addr), health(health), origin(origin), team(team),
        is_defusing(is_defusing), weapon(weapon), networkAngle(networkAngle),
        aimPunch(aimPunch), viewOrigin(viewOrigin) {}
  addr_type entity_addr;
  int health;
  Vector origin;
  Team team;
  bool is_defusing;
  Weapon weapon;
  QAngle networkAngle;
  QAngle aimPunch;
  Vector viewOrigin;
};

using PlayerPtr = std::shared_ptr<Player>;

template <class T> class CUtlVector {
public:
  T *DataPtr;               // 0000 (054612C0)
  unsigned int Max;         // 0004 (054612C4)
  unsigned int unk02;       // 0008 (054612C8)
  unsigned int Count;       // 000C (054612CC)
  unsigned int DataPtrBack; // 0010 (054612D0)
};

struct GlowObjectDefinition_t {
  int m_nNextFreeSlot;
  void *m_pEntity;
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
  unsigned char unk2[4];

  static const int END_OF_FREE_LIST = -1;
  static const int ENTRY_IN_USE = -2;
}; // sizeof() == 0x34

#define MAX_BONES 128

struct BoneInfo {
  float x;
  float y;
  float z;
  char __pad0x0[0x14];
};

class GlowObjectManager_t {
public:
  CUtlVector<GlowObjectDefinition_t> objects; // 0000
  int m_nFirstFreeSlot;                       // 0014 (054612D4)
  unsigned int unk1;                          // 0018 (054612D8)
  unsigned int unk2;                          // 001C (054612DC)
  unsigned int unk3;                          // 0020 (054612E0)
  unsigned int unk4;                          // 0024 (054612E4)
  unsigned int unk5;                          // 0028 (054612E8)
};

enum Bone {
  PELVIS = 0,
  SPINE0 = 1,
  SPINE1 = 2,
  SPINE2 = 3,
  SPINE3 = 4,
  NECK = 5,
	HEAD_0 = 6,
	CLAVICLE_L= 7,
	ARM_UPPER_L = 8,
	ARM_LOWER_L = 9,
	HAND_L    = 10,
	WEAPON_HAND_L = 11,
	CLAVICLE_R  = 12,
	ARM_UPPER_R = 13,
	ARM_LOWER_R = 14,
	HAND_R  = 15,
	WEAPON_HAND_R = 16,
	BACK =  17, //JIGGLE_PRIMARY
	TORSO = 18, //WEAPONHIER_JNT
	TORSO2 = 19, //JIGGLE_SECONDARY
	WRIST_R = 20,
	WRIST_L = 21,
	LEG_UPPER_R = 22,
	LEG_LOWER_R = 23,
	ANKLE_R = 24,
	LEG_UPPER_L = 25,
	LEG_LOWER_L = 26,
	ANKLE_L = 27,
	ROOT_MOTION = 28,
	LEG_L_OFFSET  = 29,
	LEG_L_IKTARGET  = 30,
	LEG_R_OFFSET  = 31,
	LEG_R_IKTARGET  = 32,
	EYEBALL_L = 33,
	EYEBALL_R = 34,
	EYE_TARGET  = 35,
	NAPE, //JIGGLE_HOOD
	BONE_COUNT
};

// m_fFlags
// source:
// https://github.com/pmrowla/hl2sdk-csgo/blob/master/public/const.h#L110
#define FL_ONGROUND (1 << 0)  // At rest / on the ground
#define FL_DUCKING (1 << 1)   // Player flag -- Player is fully crouched
#define FL_WATERJUMP (1 << 3) // player jumping out of water
#define FL_ONTRAIN                                                             \
  (1 << 4) // Player is _controlling_ a train, so movement commands should be
           // ignored on client during prediction.
#define FL_INRAIN (1 << 5) // Indicates the entity is standing in rain
#define FL_FROZEN (1 << 6) // Player is frozen for 3rd person camera
#define FL_ATCONTROLS                                                          \
  (1 << 7) // Player can't move, but keeps key inputs for controlling another
           // entity
#define FL_CLIENT (1 << 8) // Is a player
#define FL_FAKECLIENT                                                          \
  (1 << 9) // Fake client, simulated server side; don't send network messages to
           // them
#define FL_INWATER (1 << 10) // In water
// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) --
// Can still be applied to players, though
#define FL_FLY                                                                 \
  (1 << 11) // Changes the SV_Movestep() behavior to not need to be on ground
#define FL_SWIM                                                                \
  (1 << 12) // Changes the SV_Movestep() behavior to not need to be on ground
            // (but stay in water)
#define FL_CONVEYOR (1 << 13)
#define FL_NPC (1 << 14)
#define FL_GODMODE (1 << 15)
#define FL_NOTARGET (1 << 16)
#define FL_AIMTARGET                                                           \
  (1 << 17) // set if the crosshair needs to aim onto the entity
#define FL_PARTIALGROUND (1 << 18) // not all corners are valid
#define FL_STATICPROP (1 << 19)    // Eetsa static prop!
#define FL_GRAPHED                                                             \
  (1 << 20) // worldgraph has this ent listed as something that blocks a
            // connection
#define FL_GRENADE (1 << 21)
#define FL_STEPMOVEMENT                                                        \
  (1 << 22) // Changes the SV_Movestep() behavior to not do any processing
#define FL_DONTTOUCH                                                           \
  (1 << 23) // Doesn't generate touch functions, generates Untouch() for
            // anything it was touching when this flag was set
#define FL_BASEVELOCITY                                                        \
  (1 << 24) // Base velocity has been applied this frame (used to convert base
            // velocity into momentum)
#define FL_WORLDBRUSH                                                          \
  (1 << 25) // Not moveable/removeable brush entity (really part of the world,
            // but represented as an entity for transparency or something)
#define FL_OBJECT                                                              \
  (1 << 26) // Terrible name. This is an object that NPCs should see. Missiles,
            // for example.
#define FL_KILLME                                                              \
  (1 << 27) // This entity is marked for death -- will be freed by game DLL
#define FL_ONFIRE (1 << 28)     // You know...
#define FL_DISSOLVING (1 << 29) // We're dissolving!
#define FL_TRANSRAGDOLL                                                        \
  (1 << 30) // In the process of turning into a client side ragdoll.
#define FL_UNBLOCKABLE_BY_PLAYER (1 << 31)
