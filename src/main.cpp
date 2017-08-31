#include "memory_access.hpp"
#include "typedef.hpp"
#include "manager.hpp"
#include "visu.hpp"
#include "trigger.hpp"
#include "aimer.hpp"
#include "settings.hpp"
#include "bunnyhop.hpp"

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
  bool use_aimbot = false;
  bool debug = false;
  bool use_bhop = false;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-radar")) {
      cout << "Enabled: Radar" << endl;
      use_radar = true;
    }
    else if (!strcmp(argv[i], "-trigger")) {
      cout << "Enabled: Trigger" << endl;
      use_trigger = true;
    }
    else if (!strcmp(argv[i], "-debug")) {
      debug = true;
      cout << "Enabled: Debugging" << endl;
    }
    else if (!strcmp(argv[i], "-aimbot")) {
      use_aimbot = true;
      cout << "Enabled: Aimbot" << endl;
    }
    else if (!strcmp(argv[i], "-bhop")) {
      use_bhop = true;
      cout << "Enabled: Bunnyhop" << endl;
    }
  }

  Settings settings("settings.cfg");
  debug = debug || settings.debug;
  settings.debug = debug;
  if (debug) {
    settings.print();
  }
  MemoryAccess mem(&settings);
  GameManager csgo = GameManager(mem);

  if (use_radar) {
    string map_name = "";
    if (settings.find_map) {
      if (debug) cout << "Scanning for map..." << endl;
      while (map_name == "") {
        map_name = mem.getMapName();
        this_thread::sleep_for(chrono::milliseconds(300));
      }
    } else {
      cout << "Map detection deactivated. Please choose map:" << endl;
      cin >> map_name;
    }
    cout << "Found Map: " << map_name << endl;
    Visu visu(map_name);
    visu.start();
  }


  Trigger trigger(csgo);
  Aimer aimer(csgo);
  BunnyHopper bhopper(csgo);
  if (use_bhop) {
    boost::thread triggerThread(boost::bind(&BunnyHopper::jumpLoop, &bhopper));
  }
  if (use_trigger) {
    boost::thread triggerThread(boost::bind(&Trigger::triggerLoop, &trigger));
  }

  while (true) {
    csgo.grabPlayers();
    if (debug) {
      csgo.printPlayers();
      // csgo.printEntities();
    }
    if (use_radar)
      csgo.printPlayerLocationsToFile("/tmp/locs.csv");
    if(use_aimbot) {
      vector<EntityType*> players = csgo.getPlayers();
      aimer.xSetAim(players[1]);
      // aimer.moveAim(50, 50);
    }
    this_thread::sleep_for(chrono::milliseconds(settings.main_loop_sleep));
  }
  return 0;
}
