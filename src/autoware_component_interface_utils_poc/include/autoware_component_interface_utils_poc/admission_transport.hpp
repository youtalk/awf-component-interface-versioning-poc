#pragma once

#include <rclcpp/qos.hpp>

#include "autoware_common_msgs_poc/msg/admission_result.hpp"
#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <cstddef>

// Single source of truth for the admission meta-protocol transport: the topics
// over which the versioning framework itself exchanges manifests and verdicts.
//
// These are NOT application InterfaceSpecs and deliberately do not flow through
// NodeAdaptor's register_*/broadcast path — that path would make the transport
// describe itself as an admittable interface (a layering inversion). Both the
// broadcasting side (NodeInterface) and the admission node create their plain
// rclcpp endpoints from the contracts below so the topic name, message type and
// QoS policy live in exactly one place.
namespace autoware::component_interface_utils_poc
{

// Shared QoS policy for every transport topic: reliable + transient_local so a
// late-joining node still observes the latest sample. Depth is per-endpoint.
inline rclcpp::QoS admission_transport_qos(std::size_t depth)
{
  rclcpp::QoS qos(depth);
  qos.reliable().transient_local();
  return qos;
}

// /system/interface_version — every node broadcasts its InterfaceManifest here
// and the admission node aggregates them. The publisher latches one manifest
// per node (depth 1); the aggregating subscriber retains one-per-publisher
// (depth 100), hence the deliberate depth asymmetry that a single symmetric
// InterfaceSpec could not express.
struct InterfaceVersionTransport
{
  using Message = autoware_common_msgs_poc::msg::InterfaceManifest;
  static constexpr char name[] = "/system/interface_version";
  static constexpr std::size_t publisher_depth = 1;
  static constexpr std::size_t subscriber_depth = 100;
};

// /system/admission_result — the admission node publishes one AdmissionResult
// per consumer<-provider interface pairing.
struct AdmissionResultTransport
{
  using Message = autoware_common_msgs_poc::msg::AdmissionResult;
  static constexpr char name[] = "/system/admission_result";
  static constexpr std::size_t publisher_depth = 10;
};

}  // namespace autoware::component_interface_utils_poc
