#include "radar.hpp"

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;
Radar::Radar(GameManager& csgo) : csgo(csgo),
                                  settings(Settings::getInstance()){}

Radar::~Radar() {
}

void Radar::start() {
  // first find map
  string map_name = "";
  if (settings.find_map) {
    if (settings.debug) cout << "Scanning for map..." << endl;
    while (map_name == "") {
      map_name = csgo.getMapName();
      this_thread::sleep_for(chrono::milliseconds(1000));
    }
  } else {
    cout << "Map detection deactivated. Please choose map:" << endl;
    cin >> map_name;
  }
  cout << "Found Map: " << map_name << endl;

  //start visualization
  string cmd = "python visu.py " + map_name + " " + to_string(settings.radar_sleep);
  handle = popen(cmd.c_str(), "w");
  run = true;
  writeLocations = boost::thread(boost::bind(&Radar::writeFunc, this));
}

void Radar::stop(){
  run = false;
  writeLocations.join();
  fputs("quit\n", handle);
  fflush(handle);
  pclose(handle);
}

void Radar::writeFunc() {
  while (run) {
    this_thread::sleep_for(chrono::milliseconds(settings.radar_sleep));
    const char SEPERATOR[] = "|";
    vector<EntityType*> players = csgo.getPlayers();
    if (players.empty())
      continue;
    // print local player index into array[0,0]
    string first_line = to_string(csgo.getLocalPlayerIndex()) + ",0,0,0,0,0";
    fputs(first_line.c_str(), handle);
    // Format: number,hp,team,x,y,z
    int i = 0;
    for (EntityType* player : players) {
      fputs(SEPERATOR, handle);
      ostringstream os;
      os << i << ",";
      os << player->m_iHealth << ",";
      os << player->m_iTeamNum << ",";
      os << player->m_vecOrigin.x << ",";
      os << player->m_vecOrigin.y << ",";
      os << player->m_vecOrigin.z;
      fputs(os.str().c_str(), handle);
      i++;
    }
    fputs("\n", handle);
    fflush(handle);
  }
}
