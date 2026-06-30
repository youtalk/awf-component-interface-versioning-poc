#pragma once

#include "autoware_component_interface_specs_poc/perception.hpp"
#include "autoware_component_interface_specs_poc/version.hpp"
#include "autoware_component_interface_utils_poc/node_interface.hpp"

#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <cstdint>
#include <string>

namespace interface_versioning_demo_nodes
{

// Build the demo component's InterfaceManifest WITHOUT a running node, reusing the exact
// register_* calls the runtime nodes use. This is what gets baked into the image, so the
// baked manifest equals what the node would broadcast (minus remap, which is out of scope here:
// resolved_name defaults to Spec::name).
inline autoware_common_msgs_poc::msg::InterfaceManifest build_manifest(
  const std::string & role, std::uint16_t consumer_major)
{
  namespace specs = autoware::component_interface_specs_poc;
  namespace utils = autoware::component_interface_utils_poc;

  utils::NodeInterface iface;
  if (role == "provider") {
    iface.register_provided<specs::perception::ObjectRecognition>();
  } else {
    iface.register_required<specs::perception::ObjectRecognition>(
      specs::accept_major(consumer_major));
  }
  auto manifest = iface.manifest();  // copy; owner/node_name are set by broadcast() at runtime
  manifest.owner = "autowarefoundation";
  manifest.node_name = (role == "provider") ? "/provider" : "/consumer";
  return manifest;
}

}  // namespace interface_versioning_demo_nodes
