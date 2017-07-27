#include "typedef.hpp"
#include <vector>
#include <string>
class Visu {
private:
  const std::string& map_name;
public:
  Visu(const std::string&);
  ~Visu();
  void start();
};
