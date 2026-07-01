#pragma once

#include "autoware_component_interface_specs_poc/perception.hpp"
#include "autoware_component_interface_specs_poc/version.hpp"
#include "autoware_component_interface_utils_poc/node_interface.hpp"

#include <cstdint>
#include <string>

namespace interface_versioning_demo_nodes
{

// Single source of truth for WHICH interfaces the demo component provides / requires per role.
// build_manifest (the static image bake) registers exactly these on a bare NodeInterface. The
// live provider_node / consumer_node create the same interfaces via NodeAdaptor::create_* (which
// additionally captures the remap-resolved name at runtime — that is why the nodes keep using
// create_* rather than this register-only helper). Change an interface here and in the matching
// node's create_* call; this header is the canonical role→interface list for the static side.
inline void register_role_interfaces(
  autoware::component_interface_utils_poc::NodeInterface & iface, const std::string & role,
  std::uint16_t consumer_major)
{
  namespace specs = autoware::component_interface_specs_poc;
  if (role == "provider") {
    iface.register_provided<specs::perception::ObjectRecognition>();
  } else {
    iface.register_required<specs::perception::ObjectRecognition>(
      specs::accept_major(consumer_major));
  }
}

}  // namespace interface_versioning_demo_nodes
