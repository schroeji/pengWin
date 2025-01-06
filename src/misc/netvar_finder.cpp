#include "netvar_finder.hpp"
#include "util.hpp"
#include <cstring>
#include <string>
#include <vector>

std::string readString(std::vector<std::uint8_t>::const_iterator iter) {
  std::string result{};
  while (*iter != 0) {
    result += *iter;
    std::advance(iter, 1);
  }
  return result;
}
std::uint64_t read_uint64(std::uint8_t *const addr) {
  std::uint64_t result;
  std::memcpy(&result, addr, sizeof(result));
  return result;
}

std::uint32_t read_uint32(std::uint8_t *const addr) {
  std::uint32_t result;
  std::memcpy(&result, addr, sizeof(result));
  return result;
}

void NetvarFinder::insert(std::string name, std::uint8_t *const addr,
                          bool network_enable) {
  if (netvar_map_.find(name) != netvar_map_.end() || name.length() > 40) {
    return;
  }
  if (name == "m_sSanitizedPlayerName" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_hPawn" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_hOwnerEntity" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_iHealth" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_ArmorValue" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_iTeamNum" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_lifeState" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_pClippingWeapon") {
    netvar_map_.insert(
        std::pair<std::string, addr_type>(name, read_uint32(addr + 0x10)));
  } else if (name == "m_flFOVSensitivityAdjust") {
    netvar_map_.insert(
        std::pair<std::string, addr_type>(name, read_uint32(addr + 0x10)));
  } else if (name == "m_pGameSceneNode") {
    netvar_map_.insert(
        std::pair<std::string, addr_type>(name, read_uint32(addr + 0x10)));
  } else if (name == "m_vecViewOffset" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x10 + 0x08)));
  } else if (name == "m_vecVelocity") {
    auto offset = read_uint32(addr + 0x08);
    if (800 < offset && offset < 1600) {
      netvar_map_.insert(std::pair<std::string, addr_type>(name, offset));
    }
  } else if (name == "m_aimPunchCache" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_iShotsFired" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "v_angle") {
    netvar_map_.insert(
        std::pair<std::string, addr_type>(name, read_uint32(addr + 0x08)));
  } else if (name == "m_entitySpottedState" && network_enable) {
    auto offset = read_uint32(addr + 0x08 + 0x10);
    if (10000 < offset && offset < 14000) {
      netvar_map_.insert(std::pair<std::string, addr_type>(name, offset));
    }
  } else if (name == "m_pObserverServices") {
    netvar_map_.insert(
        std::pair<std::string, addr_type>(name, read_uint32(addr + 0x08)));
  } else if (name == "m_bDormant") {
    netvar_map_.insert(
        std::pair<std::string, addr_type>(name, read_uint32(addr + 0x08)));
  } else if (name == "m_vecAbsOrigin" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_modelState") {
    netvar_map_.insert(
        std::pair<std::string, addr_type>(name, read_uint32(addr + 0x08)));
  } else if (name == "m_bSpotted") {
    netvar_map_.insert(
        std::pair<std::string, addr_type>(name, read_uint32(addr + 0x10)));
  } else if (name == "m_bSpttedByMask" && network_enable) {
    netvar_map_.insert(std::pair<std::string, addr_type>(
        name, read_uint32(addr + 0x08 + 0x10)));
  } else if (name == "m_hObserverTarget") {
    netvar_map_.insert(
        std::pair<std::string, addr_type>(name, read_uint32(addr + 0x08)));
  } else {
    // if (name.substr(0, 2) == "m_") {
    //   std::cout << "Ignored netvar " << name << std::endl;
    // }
  }
}

addr_type NetvarFinder::getNetvar(std::string const &name) const {
  return netvar_map_.at(name);
}
void NetvarFinder::dump() {
  auto client_range = mem_.getClientRange();
  auto const base = client_range[0].first;
  auto module_size = mem_.getModuleSize(base);
  cout << "Module has size: " << module_size << std::endl;
  std::vector<std::uint8_t> client_dump{};
  client_dump.resize(module_size);
  std::cout << "Reserved " << module_size << "bytes of mem_ory" << std::endl;
  if (mem_.read(base, client_dump.data(), module_size)) {
    std::cout << "Dumped module." << std::endl;
  } else {
    std::cout << "Failed to dump module." << std::endl;
    return;
  }
  for (unsigned long i{module_size - 8}; i > 0; i -= 8) {
    bool network_enable = false;
    addr_type name_pointer{read_uint64(&client_dump[i])};
    if ((name_pointer >= base) && (name_pointer <= base + module_size)) {
      name_pointer = read_uint64(&client_dump[name_pointer - base]);
      if ((name_pointer >= base) && (name_pointer <= base + module_size)) {
        auto name_iter = client_dump.begin();
        std::advance(name_iter, name_pointer - base);
        auto name = readString(name_iter);
        std::transform(name.begin(), name.end(), name.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (name == "mnetworkenable") {
          network_enable = true;
        }
      }
    }

    addr_type name_ptr{network_enable ? read_uint64(&client_dump[i + 0x08])
                                      : read_uint64(&client_dump[i])};
    if ((name_ptr < base) || (name_ptr > base + module_size)) {
      continue;
    }
    auto name_iter = client_dump.begin();
    std::advance(name_iter, name_ptr - base);
    std::string netvar_name = readString(name_iter);
    insert(netvar_name, &client_dump[i], network_enable);
  }
}

std::map<std::string, std::uint64_t> const &NetvarFinder::getNetvars() const {
  return netvar_map_;
}

void NetvarFinder::printNetvars() {
  std::cout << "Found netvars:" << std::endl;
  for (const auto &pair : netvar_map_) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }
}
