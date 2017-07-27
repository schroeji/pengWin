
#include "visu.hpp"
#include <vector>
#include <iostream>

using namespace std;
Visu::Visu(const string& map_name) : map_name(map_name) {
  // Py_Initialize();
  // PyRun_SimpleString(
  // "import matplotlib.pyplot as plt\n"
  // "import matplotlib.animation as animation\n"
  // "import numpy as np\n"
  // "fig = plt.figure()\n"
  // "ax = fig.add_subplot(1,1,1)\n");
}

Visu::~Visu() {
  // Py_Finalize();
}

void Visu::start() {
  string cmd = "python visu.py " + map_name;
  popen(cmd.c_str(), "r");
}
