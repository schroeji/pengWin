#include "radar.hpp"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace std;
Radar::Radar(GameManager &csgo)
    : csgo(csgo), settings(Settings::getInstance()) {
  running = false;
}

Radar::~Radar() {}

void Radar::start() {
  // first find map
  string map_name = csgo.getMapName();

  // start visualization
  string cmd =
      "python3 visu.py " + map_name + " " + to_string(settings.radar_sleep);
  handle = popen(cmd.c_str(), "w");
  running = true;
  writeLocations = boost::thread(boost::bind(&Radar::writeFunc, this));
}

void Radar::stop() {
  if (running) {
    running = false;
    // writeLocations.join();
    fputs("quit\n", handle);
    fflush(handle);
    pclose(handle);
    if (settings.debug)
      cout << "Stopped radar..." << std::endl;
  }
}

void Radar::writeFunc() {
  while (running) {
    this_thread::sleep_for(chrono::milliseconds(settings.radar_sleep));
    const char SEPERATOR[] = "|";
    auto const players = csgo.getPlayers();
    if (players.empty())
      continue;
    // print local player index into array[0,0]
    // and rotation of local player for generic radar type
    try {
      string first_line = to_string(csgo.getLocalPlayerIndex()) +
                          ",0,0,0,0,0,0,0," +
                          to_string(csgo.getLocalPlayer()->networkAngle.x);
      fputs(first_line.c_str(), handle);
    } catch (const runtime_error &e) {
      stop();
      return;
    }
    // Format: number,hp,team,weapon,defusing,x,y,z,rotation
    int i = 0;
    for (PlayerPtr player : players) {
      fputs(SEPERATOR, handle);
      ostringstream os;
      os << i << ",";
      os << player->health << ",";
      os << player->team << ",";
      os << player->weapon << ",";
      os << player->is_defusing << ",";
      os << player->origin.x << ",";
      os << player->origin.y << ",";
      os << player->origin.z << ",";
      os << player->networkAngle.x;
      fputs(os.str().c_str(), handle);
      i++;
    }
    fputs("\n", handle);
    fflush(handle);
  }
}
