#pragma once

#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <sstream>
#include <string>

namespace autoware::interface_admission
{

// Hand-rolled JSON writer (no nlohmann) so the manifest-emitter side stays dependency-light.
// Shape mirrors the InterfaceManifest message exactly so from_json (manifest_parse.hpp) round-trips.
inline std::string to_json(const autoware_common_msgs_poc::msg::InterfaceManifest & m)
{
  std::ostringstream os;
  os << "{\"owner\":\"" << m.owner << "\""
     << ",\"node_name\":\"" << m.node_name << "\""
     << ",\"provided\":[";
  for (std::size_t i = 0; i < m.provided.size(); ++i) {
    const auto & p = m.provided[i];
    os << (i ? "," : "")
       << "{\"ns\":\"" << p.ns << "\""
       << ",\"interface_name\":\"" << p.interface_name << "\""
       << ",\"resolved_name\":\"" << p.resolved_name << "\""
       << ",\"type_name\":\"" << p.type_name << "\""
       << ",\"major\":" << p.major
       << ",\"minor\":" << p.minor
       << ",\"patch\":" << p.patch << "}";
  }
  os << "],\"required\":[";
  for (std::size_t i = 0; i < m.required.size(); ++i) {
    const auto & r = m.required[i];
    os << (i ? "," : "")
       << "{\"ns\":\"" << r.ns << "\""
       << ",\"interface_name\":\"" << r.interface_name << "\""
       << ",\"resolved_name\":\"" << r.resolved_name << "\""
       << ",\"type_name\":\"" << r.type_name << "\""
       << ",\"accept_major_min\":" << r.accept_major_min
       << ",\"accept_major_max\":" << r.accept_major_max
       << ",\"min_minor\":" << r.min_minor << "}";
  }
  os << "]}";
  return os.str();
}

}  // namespace autoware::interface_admission
