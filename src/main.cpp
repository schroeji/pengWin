#include "memory_access.hpp"
#include "typedef.hpp"
#include "manager.hpp"
#include "visu.hpp"
#include "trigger.hpp"

#include <stdio.h>
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
  } else if (argc < 2 ){
    cout << "No map specified: Visualization disabled" << endl;
  } else {
    const string map_name(argv[1]);
    Visu visu(map_name);
    visu.start();
  }
  MemoryAccess mem;
  GameManager csgo = GameManager(mem);
  Trigger trigger(csgo);
  while (true) {
    // cout << "getting Players" << endl;
    csgo.grabPlayers();
    trigger.triggerCheck();
    // csgo.printPlayers();
    // cout << "printing Players" << endl;
    // csgo.printPlayerLocationsToFile("locs.csv");
    this_thread::sleep_for(chrono::milliseconds(300));
  }
  return 0;
}
