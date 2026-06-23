#pragma once

#include "autoware_component_interface_specs_poc/concepts.hpp"

#include <rclcpp/qos.hpp>

namespace autoware::component_interface_utils_poc
{

template <autoware::component_interface_specs_poc::InterfaceSpec S>
inline rclcpp::QoS get_qos()
{
  rclcpp::QoS qos(S::depth);
  if (S::reliability == RMW_QOS_POLICY_RELIABILITY_RELIABLE) {
    qos.reliable();
  } else {
    qos.best_effort();
  }
  if (S::durability == RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL) {
    qos.transient_local();
  } else {
    qos.durability_volatile();
  }
  return qos;
}

}  // namespace autoware::component_interface_utils_poc
