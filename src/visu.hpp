#include "typedef.hpp"

#include <vector>
#include <string>
#include <iostream>

class Visu {
private:
  FILE* handle;
public:
  Visu();
  ~Visu();
  void start(const std::string&);
};
