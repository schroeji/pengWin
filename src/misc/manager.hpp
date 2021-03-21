#pragma once
#include "memory_access.hpp"
#include "typedef.hpp"
#include "util.hpp"

#include <string>
#include <vector>

class GameManager {
public:
  GameManager(MemoryAccess &mem);
  // get Players from the game
  void grabPlayers();
  // printing stuff for debugging
  void printPlayers();
  void printEntities();
  void printPlayerLocationsToFile(const std::string &);
  // return the MemoryAccess  isntance of this object
  MemoryAccess &getMemoryAccess();
  // return the players (read by the last grabPlayers call)
  std::vector<EntityType *> &getPlayers();
  // return the LocalPlayer object (read by last grabPlayers call)
  EntityType *getLocalPlayer();
  // returns the index of the localplayer in the player lsit returned by
  // getPlayers
  int getLocalPlayerIndex();
  // checks if the game is running
  bool gameRunning();
  // checks if the game is currebtly conencted to a a server
  bool isOnServer();
  // returns the player address for a player object
  addr_type getPlayerAddr(EntityType *);
  // returns the current map name
  std::string getMapName();
  // returns if the player at player_addr is scoped
  bool isScoped(addr_type);
  // returns if the player at player_addr is defusing
  bool isDefusing(addr_type);
  // returns if the player at player_addr is flashed
  bool isFlashed(addr_type);
  // returns the total duration of the flashed status for the player
  float getFlashDuration(addr_type);
  // returns if the local player is flashed because it is used in the aimbot it
  // does not wait until the flash is fully gone but until the player can see
  // again
  bool isLocalPlayerFlashed();
  // returns the weapon the player is using
  Weapon getWeapon(addr_type);
  // returns the team the player belongs to
  Team getTeam(addr_type);
  // returns the team the crosshair target of the player
  unsigned int getCrosshairTarget(addr_type);
  QAngle getAimPunch(addr_type);
  // returns the current network angles (data is read directly from memory and
  // not from the player list)
  // => more recent
  QAngle getNetworkAngles(addr_type);
  // returns a vector of smoke locations i.e. the centers of blooming smokes
  std::vector<Vector> getSmokeLocations();
  // returns if the line connecting start and end passes through a smoke
  bool lineThroughSmoke(Vector start, Vector end);

private:
  MemoryAccess &mem;
  Settings &settings;
  std::vector<EntityType *> players;
  std::vector<EntityType *> nonPlayerEntities;
  std::vector<addr_type> player_addrs;
  // number of the local_player in players
  int local_player_index = -1;
  EntityType *local_player = new EntityType;
  bool connected = false;
  GlowObjectManager_t manager;
  GlowObjectDefinition_t g_glow[1024];
  std::string current_map_;

  // some helper variables
  float local_player_flashed_timer = 0.0;
};
