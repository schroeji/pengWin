#include "misc/memory_access.hpp"
#include "misc/typedef.hpp"
#include "misc/manager.hpp"
#include "misc/settings.hpp"
#include "misc/hotkey.hpp"
#include "misc/util.hpp"
#include "hacks/radar.hpp"
#include "hacks/trigger.hpp"
#include "hacks/aimer.hpp"
#include "hacks/bunnyhop.hpp"

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
using namespace std;

void printUsage(const string& name, const string& config_file) {
  cout << "usage: ";
  cout << name << " [options]" << endl;
  cout << "Possible options:" << endl;
  cout << "-a, --aimbot    Enable Aimbot" << endl;
  cout << "-b, --bhop      Enable Bunnyhop" << endl;
  cout << "-d, --debug     Enable debugging" << endl;
  cout << "-r, --radar     Enable external Radar" << endl;
  cout << "-t, --trigger   Enable Triggerbot" << endl;
  cout << endl;
  cout << "For advanced configuration use: " << config_file << endl;;
}

int main(int argc, char** argv) {
  string config_file = "settings.cfg";
  if (getuid() != 0){
    cout << "Not root" << endl;
    return 0;
  }
  bool use_radar = false;
  bool use_trigger = false;
  bool use_aimbot = false;
  bool debug = false;
  bool use_bhop = false;
  bool panicked = false;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--radar") || !strcmp(argv[i], "-r")) {
      cout << "Enabled: Radar" << endl;
      use_radar = true;
    }
    else if (!strcmp(argv[i], "--trigger") || !strcmp(argv[i], "-t")) {
      cout << "Enabled: Trigger" << endl;
      use_trigger = true;
    }
    else if (!strcmp(argv[i], "--debug") || !strcmp(argv[i], "-d")) {
      debug = true;
      cout << "Enabled: Debugging" << endl;
    }
    else if (!strcmp(argv[i], "--aimbot") || !strcmp(argv[i], "-a")) {
      use_aimbot = true;
      cout << "Enabled: Aimbot" << endl;
    }
    else if (!strcmp(argv[i], "--bhop") || !strcmp(argv[i], "-b")) {
      use_bhop = true;
      cout << "Enabled: Bunnyhop" << endl;
    }
  }
  if(! (use_aimbot || use_bhop || debug || use_trigger || use_radar)) {
    cout << "Please give at least one valid argument." << endl;
    printUsage(string(argv[0]), config_file);
    return 0;
  }

  Settings settings(config_file.c_str());
  debug = debug || settings.debug;
  settings.debug = debug;
  settings.print();
  MemoryAccess mem(&settings);
  GameManager csgo = GameManager(mem);

  Trigger trigger(csgo);
  Aimer aimer(csgo);
  BunnyHopper bhopper(csgo);
  Radar radar(csgo);
  HotkeyManager hotkeyMan(csgo);

  while (!panicked && csgo.gameRunning()) {
    if (debug) cout << "Waiting until connected..." << endl;
    while (csgo.gameRunning()) {
      if (csgo.isOnServer())
        break;
      this_thread::sleep_for(chrono::milliseconds(3000));
    }
    if (!csgo.gameRunning())
      break;

    if (debug) cout << "Connected to a server..." << endl;

    if (use_radar) radar.start();

    if (use_bhop) {
      boost::function<void(unsigned int)> bhopFunc = boost::bind(&BunnyHopper::jumpCheck, &bhopper, _1);
      hotkeyMan.bind(settings.bhop_key, bhopFunc);
    }
    if (use_trigger){
      boost::function<void(unsigned int)> triggerFunc = boost::bind(&Trigger::triggerCheck, &trigger, _1);
      hotkeyMan.bind(settings.trigger_key, triggerFunc);
    }
    if (use_aimbot) {
      boost::function<void(unsigned int)> aimFunc = boost::bind(&Aimer::aimCheck, &aimer, _1);
      hotkeyMan.bind(settings.aim_key, aimFunc);
    }
    // function for panic key to stop everything
    boost::function<void(unsigned int)> stop = [&hotkeyMan, &radar, &panicked](unsigned int x) {
      hotkeyMan.stopListen();
      radar.stop();
      if (x == 0)
        panicked = true;
    };
    hotkeyMan.bind(settings.panic_key, stop);
    hotkeyMan.startListen();

    // main loop
    while (!panicked && csgo.isOnServer()) {
      csgo.grabPlayers();
      if (debug) {
        csgo.printPlayers();
        // csgo.printEntities();
      }
      this_thread::sleep_for(chrono::milliseconds(settings.main_loop_sleep));
    }
    if (debug) cout << "Not on a server. Stopping everything..." << endl;
    if (!panicked) {
      stop(1);
    }
    if (debug) cout << "Stopped everything. Entering sleep mode..." << endl;
  }

  if (debug) cout << "Game closed or panic key pressed. Terminating..." << endl;
  return 0;
}
