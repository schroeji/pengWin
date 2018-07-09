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
