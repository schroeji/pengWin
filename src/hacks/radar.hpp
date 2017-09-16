#include "misc/typedef.hpp"
#include "misc/manager.hpp"
#include "misc/settings.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <boost/thread.hpp>

class Radar {

private:
  FILE* handle;
  boost::thread writeLocations;
  bool run = true;
  void writeFunc();
  GameManager& csgo;
  Settings settings;

public:
  Radar(GameManager&);
  ~Radar();
  void start(const std::string&);
  void stop();
};
