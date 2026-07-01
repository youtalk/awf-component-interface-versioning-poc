#pragma once

#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <utility>

namespace autoware::interface_admission
{

// Serialize an InterfaceManifest to JSON via nlohmann::json — already a required dependency of
// this package (from_json uses it), so string fields are escaped correctly and the key names
// stay a single source of truth shared with from_json (manifest_parse.hpp).
inline std::string to_json(const autoware_common_msgs_poc::msg::InterfaceManifest & m)
{
  nlohmann::json j;
  j["owner"] = m.owner;
  j["node_name"] = m.node_name;
  j["provided"] = nlohmann::json::array();
  for (const auto & p : m.provided) {
    nlohmann::json pj;
    pj["ns"] = p.ns;
    pj["interface_name"] = p.interface_name;
    pj["resolved_name"] = p.resolved_name;
    pj["type_name"] = p.type_name;
    pj["major"] = p.major;
    pj["minor"] = p.minor;
    pj["patch"] = p.patch;
    j["provided"].push_back(std::move(pj));
  }
  j["required"] = nlohmann::json::array();
  for (const auto & r : m.required) {
    nlohmann::json rj;
    rj["ns"] = r.ns;
    rj["interface_name"] = r.interface_name;
    rj["resolved_name"] = r.resolved_name;
    rj["type_name"] = r.type_name;
    rj["accept_major_min"] = r.accept_major_min;
    rj["accept_major_max"] = r.accept_major_max;
    rj["min_minor"] = r.min_minor;
    j["required"].push_back(std::move(rj));
  }
  return j.dump();
}

}  // namespace autoware::interface_admission
