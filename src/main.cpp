#include "memory_access.hpp"
#include "typedef.hpp"
#include "manager.hpp"
#include "visu.hpp"
#include "trigger.hpp"

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv) {
  if (getuid() != 0){
    cout << "Not root" << endl;
    return 0;
  }
  bool use_radar = false;
  bool use_trigger = false;
  bool debug = false;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-radar")) {
      cout << "enabled radar" << endl;
      use_radar = true;
    }
    else if (!strcmp(argv[i], "-trigger")) {
      cout << "trigger enabled" << endl;
      use_trigger = true;
    }
    else if (!strcmp(argv[i], "-debug")) {
      debug = true;
      cout << "enabled debugging" << endl;
    }
  }

  MemoryAccess mem;
  GameManager csgo = GameManager(mem);
  if (use_radar) {
    string map_name = "";
    while (map_name == "") {
      map_name = mem.getMapName();
      this_thread::sleep_for(chrono::milliseconds(300));
    }
    cout << "Found Map: " << map_name << endl;
    Visu visu(map_name);
    visu.start();
  }

  Trigger trigger(csgo);
  while (true) {
    csgo.grabPlayers();
    if (use_trigger)
      trigger.triggerCheck();
    if (debug)
      csgo.printPlayers();
    if (use_radar)
      csgo.printPlayerLocationsToFile("/tmp/locs.csv");
    this_thread::sleep_for(chrono::milliseconds(300));
  }
  return 0;
}
