#include "typedef.hpp"

#include <vector>
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

std::vector<std::string> split_string(const std::string& split_str, const std::string& delimiter) {
  const char* str = split_str.c_str();
  const char* deli = delimiter.c_str();
  std::vector<std::string> result;
  std::string buf = "";
  for (unsigned int i = 0; i < split_str.length(); i++){
    if (!strncmp(&str[i], deli, delimiter.length())) {
      result.push_back(buf);
      buf = "";
    } else {
      buf += str[i];
    }
  }
  result.push_back(buf);
  return result;
}
