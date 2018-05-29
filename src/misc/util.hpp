#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <math.h>
#include <vector>

// according to http://counterstrike.wikia.com/wiki/Smoke_Grenade
// radius should be 144 we use less because edges are not straight
#define smokeRadius 135.0

enum Team {
  CT = 3,
  T = 2
};

struct MouseMovement {
  int x;
  int y;
};

typedef std::pair<unsigned long int, unsigned long int> Addr_Range;
typedef unsigned long int addr_type;

struct Vector2D {
  float x;
  float y;
  inline Vector2D operator+(Vector2D a) {
    return {a.x + x, a.y + y};
  }
  inline float operator*(Vector2D a) {
    return a.x * x + a.y * y;
  }
  inline Vector2D operator*(float a) {
    return {a*x, a*y};
  }
};

struct Vector {
	float x;
	float y;
	float z;
  inline Vector operator+(Vector a) {
    return {a.x + x, a.y + y, a.z + z};
  }
  inline float operator*(Vector a) {
    return a.x * x + a.y * y + a.z * z;
  }
  inline Vector operator-(Vector a) {
    return {x - a.x, y - a.y, z - a.z};
  }
  inline Vector operator*(float a) {
    return {a*x, a*y, a*z};
  }
};

struct QAngle {
	float x; // Pitch
	float y; // Yaw
	float z; // Roll
  inline QAngle operator+(QAngle a) {
    return {a.x + x, a.y + y, a.z + z};
  }
  inline QAngle operator-(QAngle a) {
    return {x - a.x, y - a.y, z - a.z};
  }
  inline QAngle operator*(float a) {
    return {a*x, a*y, a*z};
  }
};

std::vector<std::string> split_string(const std::string&, const std::string&);
void normalize_vector(Vector*);
void normalize_vector(Vector2D*);
Vector getDist(Vector*, Vector*);
float scalar_prod(Vector*, Vector*);
float scalar_prod(Vector2D*, Vector2D*);
float sgn(float);

inline float degree_to_radian(float x) {
  return x*0.01745329;
}
inline float radian_to_degree(float x) {
  return x/0.01745329;
}

inline float len(Vector2D vec) {
  return sqrt(vec.x * vec.x + vec.y * vec.y);
}

inline float len(Vector vec) {
  return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline int len(MouseMovement move) {
  // manhatten distance
  return move.x + move.y;
}
