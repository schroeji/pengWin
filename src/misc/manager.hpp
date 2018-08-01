#pragma once
#include "memory_access.hpp"
#include "typedef.hpp"
#include "util.hpp"

#include <string>
#include <vector>

class GameManager {
public:
  GameManager(MemoryAccess& mem);
  // get Players from the game
  void grabPlayers();
  // printing stuff for debugging
  void printPlayers();
  void printEntities();
  void printPlayerLocationsToFile(const std::string&);
  // return the MemoryAccess  isntance of this object
  MemoryAccess& getMemoryAccess();
  // return the players (read by the last grabPlayers call)
  std::vector<EntityType*>& getPlayers();
  // return the LocalPlayer object (read by last grabPlayers call)
  EntityType* getLocalPlayer();
  // returns the index of the localplayer in the player lsit returned by getPlayers
  int getLocalPlayerIndex();
  // checks if the game is running
  bool gameRunning();
  // checks if the game is currebtly conencted to a a server
  bool isOnServer();
  // returns the player address for a player object
  addr_type getPlayerAddr(EntityType*);
  // returns the current map name
  std::string getMapName();

  bool isScoped(addr_type);
  bool isDefusing(addr_type);
  Weapon getWeapon(addr_type);
  Team getTeam(addr_type);
  unsigned int getCrosshairTarget(addr_type);
  QAngle getAimPunch(addr_type);
  // returns the current network angles (data is read directly from memory and not from the player list)
  // => more recent
  QAngle getNetworkAngles(addr_type);
  std::vector<Vector> getSmokeLocations();
  bool lineThroughSmoke(Vector start, Vector end);

private:
  MemoryAccess& mem;
  Settings& settings;
  std::vector<EntityType*> players;
  std::vector<EntityType*> nonPlayerEntities;
  std::vector<addr_type> player_addrs;
  // number of the local_player in players
  int local_player_index = -1;
  EntityType* local_player = new EntityType;
  bool connected = false;
  GlowObjectManager_t manager;
  GlowObjectDefinition_t g_glow[1024];
};
