#include "typedef.hpp"

#include <vector>
#include <string.h>
#include <string>
#include <iostream>
#include <math.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/algorithm/string.hpp>

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
  float length = len(*vec);
  vec->x /= length;
  vec->y /= length;
  vec->z /= length;
}

void normalize_vector(Vector2D* vec) {
  float length = len(*vec);
  vec->x /= length;
  vec->y /= length;
}

Vector getDist(Vector* a, Vector* b) {
  return Vector{b->x - a->x, b->y - a->y, b->z - a->z};
}

float scalar_prod(Vector* a, Vector* b) {
  return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

float scalar_prod(Vector2D* a, Vector2D* b) {
  return (a->x * b->x) + (a->y * b->y);
}

Vector cross_prod(Vector* a, Vector* b) {
  return {a->y*b->z - a->z*b->y, a->z*b->x - a->x*b->z, a->x*b->y - a->y*b->x};
}

float sgn(float val) {
  return (0.0 < val) - (val < 0.0);
}

Weapon getWeaponByName(std::string weapon_name){
  boost::to_upper(weapon_name);
  if (weapon_name == "DEAGLE")
    return Weapon::DEAGLE;
  if (weapon_name == "ELITE")
    return Weapon::ELITE;
  if (weapon_name == "FIVESEVEN")
    return Weapon::FIVESEVEN;
  if (weapon_name == "GLOCK")
    return Weapon::GLOCK;
  if (weapon_name == "AK47")
    return Weapon::AK47;
  if (weapon_name == "AUG")
    return Weapon::AUG;
  if (weapon_name == "AWP")
    return Weapon::AWP;
  if (weapon_name == "FAMAS")
    return Weapon::FAMAS;
  if (weapon_name == "G3SG1")
    return Weapon::G3SG1;
  if (weapon_name == "GALILAR")
    return Weapon::GALILAR;
  if (weapon_name == "M249")
    return Weapon::M249;
  if (weapon_name == "M4A4")
    return Weapon::M4A4;
  if (weapon_name == "MAC10")
    return Weapon::MAC10;
  if (weapon_name == "P90")
    return Weapon::P90;
  if (weapon_name == "UMP45")
    return Weapon::UMP45;
  if (weapon_name == "MP5SD")
    return Weapon::MP5SD;
  if (weapon_name == "XM1014")
    return Weapon::XM1014;
  if (weapon_name == "BIZON")
    return Weapon::BIZON;
  if (weapon_name == "MAG7")
    return Weapon::MAG7;
  if (weapon_name == "NEGEV")
    return Weapon::NEGEV;
  if (weapon_name == "SAWEDOFF")
    return Weapon::SAWEDOFF;
  if (weapon_name == "TEC9")
    return Weapon::TEC9;
  if (weapon_name == "ZEUS")
    return Weapon::ZEUS;
  if (weapon_name == "HKP2000")
    return Weapon::HKP2000;
  if (weapon_name == "MP7")
    return Weapon::MP7;
  if (weapon_name == "MP9")
    return Weapon::MP9;
  if (weapon_name == "NOVA")
    return Weapon::NOVA;
  if (weapon_name == "P250")
    return Weapon::P250;
  if (weapon_name == "SCAR20")
    return Weapon::SCAR20;
  if (weapon_name == "SG556")
    return Weapon::SG556;
  if (weapon_name == "SCOUT")
    return Weapon::SSG08;
  if (weapon_name == "SSG08")
    return Weapon::SSG08;
  if (weapon_name == "KNIFE")
    return Weapon::KNIFE;
  if (weapon_name == "FLASHBANG")
    return Weapon::FLASHBANG;
  if (weapon_name == "HEGRENADE")
    return Weapon::HEGRENADE;
  if (weapon_name == "SMOKEGRENADE")
    return Weapon::SMOKEGRENADE;
  if (weapon_name == "MOLOTOV")
    return Weapon::MOLOTOV;
  if (weapon_name == "DECOY")
    return Weapon::DECOY;
  if (weapon_name == "INCGRENADE")
    return Weapon::INCGRENADE;
  if (weapon_name == "C4")
    return Weapon::C4;
  if (weapon_name == "KNIFE_T")
    return Weapon::KNIFE_T;
  if (weapon_name == "M4A1")
    return Weapon::M4A1;
  if (weapon_name == "M4A1_SILENCER")
    return Weapon::M4A1;
  if (weapon_name == "USP")
    return Weapon::USP;
  if (weapon_name == "CZ75A")
    return Weapon::CZ75A;
  if (weapon_name == "REVOLVER")
    return Weapon::REVOLVER;
  if (weapon_name == "KNIFE_BAYONET")
    return Weapon::KNIFE_BAYONET;
  if (weapon_name == "KNIFE_FLIP")
    return Weapon::KNIFE_FLIP;
  if (weapon_name == "KNIFE_GUT")
    return Weapon::KNIFE_GUT;
  if (weapon_name == "KNIFE_KARAMBIT")
    return Weapon::KNIFE_KARAMBIT;
  if (weapon_name == "KNIFE_M9_BAYONET")
    return Weapon::KNIFE_M9_BAYONET;
  if (weapon_name == "KNIFE_TACTICAL")
    return Weapon::KNIFE_TACTICAL;
  if (weapon_name == "KNIFE_FALCHION")
    return Weapon::KNIFE_FALCHION;
  if (weapon_name == "KNIFE_SURVIVAL_BOWIE")
    return Weapon::KNIFE_SURVIVAL_BOWIE;
  if (weapon_name == "KNIFE_BUTTERFLY")
    return Weapon::KNIFE_BUTTERFLY;
  if (weapon_name == "KNIFE_PUSH")
    return Weapon::KNIFE_PUSH;
  return Weapon::NONE;
}

std::string getWeaponName(Weapon w) {
  switch(w) {
  case Weapon::DEAGLE:
    return "DEAGLE";
  case Weapon::ELITE:
    return "ELITE";
  case Weapon::FIVESEVEN:
    return "FIVESEVEN";
  case Weapon::GLOCK:
    return "GLOCK";
  case Weapon::AK47:
    return "AK47";
  case Weapon::AUG:
    return "AUG";
  case Weapon::AWP:
    return "AWP";
  case Weapon::FAMAS:
    return "FAMAS";
  case Weapon::G3SG1:
    return "G3SG1";
  case Weapon::GALILAR:
    return "GALILAR";
  case Weapon::M249:
    return "M249";
  case Weapon::M4A4:
    return "M4A4";
  case Weapon::MAC10:
    return "MAC10";
  case Weapon::P90:
    return "P90";
  case Weapon::UMP45:
    return "UMP45";
  case Weapon::MP5SD:
    return "MP5SD";
  case Weapon::XM1014:
    return "XM1014";
  case Weapon::BIZON:
    return "BIZON";
  case Weapon::MAG7:
    return "MAG7";
  case Weapon::NEGEV:
    return "NEGEV";
  case Weapon::SAWEDOFF:
    return "SAWEDOFF";
  case Weapon::TEC9:
    return "TEC9";
  case Weapon::ZEUS:
    return "ZEUS";
  case Weapon::HKP2000:
    return "HKP2000";
  case Weapon::MP7:
    return "MP7";
  case Weapon::MP9:
    return "MP9";
  case Weapon::NOVA:
    return "NOVA";
  case Weapon::P250:
    return "P250";
  case Weapon::SCAR20:
    return "SCAR20";
  case Weapon::SG556:
    return "SG556";
  case Weapon::SSG08:
    return "SSG08";
  case Weapon::KNIFE:
    return "KNIFE";
  case Weapon::FLASHBANG:
    return "FLASHBANG";
  case Weapon::HEGRENADE:
    return "HEGRENADE";
  case Weapon::SMOKEGRENADE:
    return "SMOKEGRENADE";
  case Weapon::MOLOTOV:
    return "MOLOTOV";
  case Weapon::DECOY:
    return "DECOY";
  case Weapon::INCGRENADE:
    return "INCGRENADE";
  case Weapon::C4:
    return "C4";
  case Weapon::KNIFE_T:
    return "KNIFE_T";
  case Weapon::M4A1:
    return "M4A1";
  case Weapon::USP:
    return "USP";
  case Weapon::CZ75A:
    return "CZ75A";
  case Weapon::REVOLVER:
    return "REVOLVER";
  case Weapon::KNIFE_BAYONET:
    return "KNIFE_BAYONET";
  case Weapon::KNIFE_FLIP:
    return "KNIFE_FLIP";
  case Weapon::KNIFE_GUT:
    return "KNIFE_GUT";
  case Weapon::KNIFE_KARAMBIT:
    return "KNIFE_KARAMBIT";
  case Weapon::KNIFE_M9_BAYONET:
    return "KNIFE_M9_BAYONET";
  case Weapon::KNIFE_TACTICAL:
    return "KNIFE_TACTICAL";
  case Weapon::KNIFE_FALCHION:
    return "KNIFE_FALCHION";
  case Weapon::KNIFE_SURVIVAL_BOWIE:
    return "KNIFE_SURVIVAL_BOWIE";
  case Weapon::KNIFE_BUTTERFLY:
    return "KNIFE_BUTTERFLY";
  case Weapon::KNIFE_PUSH:
    return "KNIFE_PUSH";
  default:
    return "NONE";
  }
}

using namespace boost::numeric::ublas;
void solve(matrix<float>* A, boost::numeric::ublas::vector<float>* b) {
  permutation_matrix<size_t> pm(A->size1());
  lu_factorize(*A, pm);
  lu_substitute(*A, pm, *b);
}
