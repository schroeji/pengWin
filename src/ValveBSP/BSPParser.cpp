#include "BSPParser.hpp"
#include "TraceRay.hpp"
#include "misc/util.hpp"
#include <mutex>

bool BSPParser::parse_map(const std::string &bsp_directory,
                          const std::string &bsp_file) {
  if (bsp_directory.empty() || bsp_file.empty()) {
    return false;
  }
  if (m_LastMap == bsp_file) {
    return true;
  }

  std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
  if (m_BSPFile.parse(bsp_directory, bsp_file)) {
    m_LastMap = bsp_file;
    return true;
  }
  return false;
}

bool BSPParser::is_visible(const Vector &origin, const Vector &destination) {
  std::shared_lock<std::shared_timed_mutex> lock(m_mutex);
  std::array<float, 3> origin_array = {origin.z, origin.x, origin.y};

  std::array<float, 3> dest_array = {destination.z, destination.x,
                                     destination.y};

  const Vector3 origin_vec3(origin_array);
  const Vector3 dest_vec3(dest_array);
  return TraceRay::is_visible(origin_vec3, dest_vec3, &m_BSPFile);
}

BSPFile BSPParser::get_bsp(void) const { return m_BSPFile; }
