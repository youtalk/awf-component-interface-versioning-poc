#pragma once

#include "autoware_component_interface_specs_poc/interface_version.hpp"
#include "autoware_component_interface_specs_poc/manifest_json.hpp"
#include "autoware_component_interface_specs_poc/version.hpp"

#include <rclcpp/rclcpp.hpp>

#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <string>

namespace autoware::component_interface_utils_poc
{

namespace specs = autoware::component_interface_specs_poc;

class NodeInterface
{
public:
  template <specs::InterfaceSpec S>
  void register_provided(std::string resolved_name = S::name)
  {
    autoware_common_msgs_poc::msg::ProvidedInterface p;
    p.ns = std::string(specs::namespace_of<S>());
    p.interface_name = S::name;
    p.resolved_name = resolved_name;
    p.type_name = specs::type_name_of<S>();
    const auto v = specs::version_of<S>();
    p.major = v.major;
    p.minor = v.minor;
    p.patch = v.patch;
    manifest_.provided.push_back(p);
  }

  template <specs::InterfaceSpec S>
  void register_required(specs::AcceptMajor accept, std::string resolved_name = S::name)
  {
    autoware_common_msgs_poc::msg::RequiredInterface r;
    r.ns = std::string(specs::namespace_of<S>());
    r.interface_name = S::name;
    r.resolved_name = resolved_name;
    r.type_name = specs::type_name_of<S>();
    r.accept_major_min = accept.min;
    r.accept_major_max = accept.max;
    r.min_minor = 0;
    manifest_.required.push_back(r);
  }

  template <specs::ServiceSpec S>
  void register_provided_service(std::string resolved_name = S::name)
  {
    autoware_common_msgs_poc::msg::ProvidedInterface p;
    p.ns = std::string(specs::namespace_of<S>());
    p.interface_name = S::name;
    p.resolved_name = resolved_name;
    p.type_name = specs::type_name_of<S>();
    const auto v = specs::version_of<S>();
    p.major = v.major;
    p.minor = v.minor;
    p.patch = v.patch;
    manifest_.provided.push_back(p);
  }

  template <specs::ServiceSpec S>
  void register_required_service(std::string resolved_name = S::name)
  {
    autoware_common_msgs_poc::msg::RequiredInterface r;
    r.ns = std::string(specs::namespace_of<S>());
    r.interface_name = S::name;
    r.resolved_name = resolved_name;
    r.type_name = specs::type_name_of<S>();
    const auto v = specs::version_of<S>();
    r.accept_major_min = v.major;
    r.accept_major_max = v.major;
    r.min_minor = 0;
    manifest_.required.push_back(r);
  }

  const autoware_common_msgs_poc::msg::InterfaceManifest & manifest() const { return manifest_; }

  void broadcast(rclcpp::Node * node, const std::string & owner)
  {
    if (pub_) { return; }  // re-entry guard: a second call is a no-op
    manifest_.owner = owner;
    manifest_.node_name = node->get_fully_qualified_name();
    // depth 1: one latched manifest per node; the admission subscriber uses depth 100 to retain one-per-publisher
    rclcpp::QoS qos(1);
    qos.reliable().transient_local();
    pub_ = node->create_publisher<autoware_common_msgs_poc::msg::InterfaceManifest>(
      "/system/interface_version", qos);
    pub_->publish(manifest_);
  }

private:
  autoware_common_msgs_poc::msg::InterfaceManifest manifest_;
  rclcpp::Publisher<autoware_common_msgs_poc::msg::InterfaceManifest>::SharedPtr pub_;
};

}  // namespace autoware::component_interface_utils_poc
