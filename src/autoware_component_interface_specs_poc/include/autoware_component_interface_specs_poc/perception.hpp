#pragma once

#include "autoware_component_interface_specs_poc/version.hpp"

#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <rmw/qos_profiles.h>

#include <cstddef>
#include <string_view>
#include <tuple>

namespace autoware::component_interface_specs_poc
{
static constexpr char owner[] = "autowarefoundation";  // GitHub org name; per-package, in one place
}

namespace autoware::component_interface_specs_poc::perception
{

static constexpr Version version{2, 1, 0};  // 0.x rules don't apply; PoC uses 2.1.0 (see spec)

// --- verbatim from autoware_component_interface_specs/perception.hpp ---
struct ObjectRecognition
{
  using Message = autoware_perception_msgs::msg::PredictedObjects;
  static constexpr char name[] = "/perception/object_recognition/objects";
  static constexpr std::size_t depth = 1;
  static constexpr auto reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  static constexpr auto durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
};
// --- end verbatim ---

// ADL hooks: one template per namespace; structs stay version/owner-free.
template <class S>
constexpr Version get_interface_version(const S &) { return version; }
template <class S>
constexpr std::string_view get_interface_namespace(const S &) { return "perception"; }

using Specs = std::tuple<ObjectRecognition>;

}  // namespace autoware::component_interface_specs_poc::perception
