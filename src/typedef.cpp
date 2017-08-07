#include "typedef.hpp"

#include <vector>
#include <string.h>
#include <string>
#include <iostream>
#include <math.h>

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

void normalize_vector(Vector* vec) {
  float squared_sum = vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;
  float root = sqrt(squared_sum);
  vec->x /= root;
  vec->y /= root;
  vec->z /= root;
}

Vector getDist(Vector* a, Vector* b) {
  return Vector{b->x - a->x, b->y - a->y, b->z - a->z};
}

float scalar_prod(Vector* a, Vector* b) {
  return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

float degree_to_radian(float x){
  return x*0.01745329;
}
