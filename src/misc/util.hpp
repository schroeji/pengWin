#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <math.h>
#include <vector>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

// according to http://counterstrike.wikia.com/wiki/Smoke_Grenade
// radius should be 144 we use less because edges are not straight
#define smokeRadius 135.0f
// no aimbot adjustment if current view vectorintersects a sphere with this radius
// around one of the target bones
#define BONE_RADIUS 4.5f
// range of zeus for aimbot
// http://counterstrike.wikia.com/wiki/Zeus_x27
#define ZEUS_RANGE 200.0f
// revolver cock time in ms
// http://counterstrike.wikia.com/wiki/R8_Revolver
#define R8_COCK_TIME 200.0f
// this defines how long before a flash bang ends a player is able to see again
// i.e. bool flashed = total_flash_duration - flashed_time > FLASH_END_OFFSET
#define FLASH_END_OFFSET 1.1f
// duration for spraying a full magazine used for FOV scaling on spray
#define MAGAZINE_DURATION 3000.0f

enum Weapon {
  NONE = 0,
  DEAGLE = 1,
  ELITE = 2,
  FIVESEVEN = 3,
  GLOCK = 4,
  AK47 = 7,
  AUG = 8,
  AWP = 9,
  FAMAS = 10,
  G3SG1 = 11,
  GALILAR = 13,
  M249 = 14,
  M4A4 = 16,
  MAC10 = 17,
  P90 = 19,
  MP5SD = 23,
  UMP45 = 24,
  XM1014 = 25,
  BIZON = 26,
  MAG7 = 27,
  NEGEV = 28,
  SAWEDOFF = 29,
  TEC9 = 30,
  ZEUS = 31,
  HKP2000 = 32,
  MP7 = 33,
  MP9 = 34,
  NOVA = 35,
  P250 = 36,
  SCAR20 = 38,
  SG556 = 39,
  SSG08 = 40,
  KNIFE = 42,
  FLASHBANG = 43,
  HEGRENADE = 44,
  SMOKEGRENADE = 45,
  MOLOTOV = 46,
  DECOY = 47,
  INCGRENADE = 48,
  C4 = 49,
  KNIFE_T = 59,
  M4A1 = 60,
  USP = 61,
  CZ75A = 63,
  REVOLVER = 64,
  KNIFE_BAYONET = 500,
  KNIFE_FLIP = 505,
  KNIFE_GUT = 506,
  KNIFE_KARAMBIT = 507,
  KNIFE_M9_BAYONET = 508,
  KNIFE_TACTICAL = 509,
  KNIFE_FALCHION = 512,
  KNIFE_SURVIVAL_BOWIE = 514,
  KNIFE_BUTTERFLY = 515,
  KNIFE_PUSH = 516
};

std::string getWeaponName(Weapon);
Weapon getWeaponByName(std::string);

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
  inline void operator+=(Vector a) {
    x += a.x;
    y += a.y;
    z += a.z;
  }
  inline Vector operator+(Vector a) {
    return {a.x + x, a.y + y, a.z + z};
  }
  // inline float operator*(Vector a) {
    // return a.x * x + a.y * y + a.z * z;
  // }
  inline float operator*(const Vector a) {
    return a.x * x + a.y * y + a.z * z;
  }
  inline Vector operator-(Vector a) {
    return {x - a.x, y - a.y, z - a.z};
  }
  inline Vector operator*(float a) {
    return {a*x, a*y, a*z};
  }
  inline Vector operator/(float a) {
    return {x/a, y/a, z/a};
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
    return {x - a.x, y - a.y, z - a.z};}
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
Vector cross_prod(Vector*, Vector*);
float sgn(float);
void solve(boost::numeric::ublas::matrix<float>*, boost::numeric::ublas::vector<float>*);

inline float degree_to_radian(float x) {
  return x*0.01745329;
}

inline float radian_to_degree(float x) {
  return x/0.01745329;
}

inline QAngle degree_to_radian(QAngle q) {
  return {degree_to_radian(q.x), degree_to_radian(q.y), degree_to_radian(q.z)};
}

inline QAngle radian_to_degree(QAngle q) {
  return {radian_to_degree(q.x), radian_to_degree(q.y), radian_to_degree(q.z)};
}

inline float len(Vector2D vec) {
  return sqrt(vec.x * vec.x + vec.y * vec.y);
}

inline float len(Vector vec) {
  return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline int len(MouseMovement move) {
  // manhatten distance
  return abs(move.x) + abs(move.y);
}

inline void printVec(std::string name, Vector vec) {
  printf("%s %f, %f, %f\n", name.c_str(), vec.x, vec.y, vec.z);
}

inline void printVec(std::string name, QAngle vec) {
  printf("%s %f, %f, %f\n", name.c_str(), vec.x, vec.y, vec.z);
}

inline bool lineSphereIntersection(Vector line_start, Vector line_end,
                                   Vector sphere_center, float sphere_radius) {
  // according to https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
  // a line and a sphere intersect if the following is >= 0
  Vector line_dir = line_end - line_start;
  normalize_vector(&line_dir);
  Vector line_start_to_sphere_center = line_start - sphere_center;
  float t2 = line_start_to_sphere_center * line_start_to_sphere_center;
  float t1 = line_dir * line_start_to_sphere_center;
  t1 = t1 * t1;
  float has_intersection = t1 - t2 + sphere_radius*sphere_radius;
  return (has_intersection >= 0); // solution exists => intersecion
}
