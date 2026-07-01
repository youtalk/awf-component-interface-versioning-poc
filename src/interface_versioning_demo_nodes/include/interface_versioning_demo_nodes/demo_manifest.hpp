#pragma once

#include "interface_versioning_demo_nodes/demo_interfaces.hpp"
#include "autoware_component_interface_utils_poc/node_interface.hpp"

#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <cstdint>
#include <string>

namespace interface_versioning_demo_nodes
{

// Build the demo component's InterfaceManifest WITHOUT a running node, from the single-source
// interface set in demo_interfaces.hpp. resolved_name defaults to Spec::name (there is no remap
// context at image-build time; remap-resolved matching is the runtime path's job).
inline autoware_common_msgs_poc::msg::InterfaceManifest build_manifest(
  const std::string & role, std::uint16_t consumer_major)
{
  namespace utils = autoware::component_interface_utils_poc;

  utils::NodeInterface iface;
  register_role_interfaces(iface, role, consumer_major);
  auto manifest = iface.manifest();  // copy; owner/node_name are set by broadcast() at runtime
  manifest.owner = "autowarefoundation";
  manifest.node_name = (role == "provider") ? "/provider" : "/consumer";
  return manifest;
}

}  // namespace interface_versioning_demo_nodes
