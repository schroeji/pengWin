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


bool test(MemoryAccess& mem) {
  unsigned long int OFFSET_CLIENTSTATE = 0x5D0214;
  unsigned long int OFFSET_MAPNAME = 0x26C;
  unsigned int Address = 0x0;
  unsigned long int engine_base = mem.getModule("engine_client.so");
  mem.read((void*)(engine_base + OFFSET_CLIENTSTATE), &Address, sizeof(Address));
  cout << hex << "Addr: " << Address << endl;
  if (!Address){ return false; }
  char MapName[256];
  if (!MapName){ return false; }
  mem.read((void*)(Address + OFFSET_MAPNAME), &MapName, sizeof(MapName));
  cout << "map_name:" << MapName << endl;
  return MapName;
}

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
  // test(mem);
  // Trigger trigger(csgo);
  while (true) {
    csgo.grabPlayers();
    // trigger.triggerCheck();
    // csgo.printPlayers();
    csgo.printPlayerLocationsToFile("/tmp/locs.csv");
    this_thread::sleep_for(chrono::milliseconds(300));
  }
  return 0;
}
