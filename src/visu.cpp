
#include "visu.hpp"
#include <vector>
#include <iostream>

using namespace std;
Visu::Visu() {}

Visu::~Visu() {
}

void Visu::start(const string& map_name) {
  string cmd = "python visu.py " + map_name;
  handle = popen(cmd.c_str(), "w");
  // pclose(in);
}
