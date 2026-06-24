#pragma once

#include "autoware_component_interface_specs_poc/interface_version.hpp"
#include "autoware_component_interface_utils_poc/node_interface.hpp"
#include "autoware_component_interface_utils_poc/qos.hpp"

#include <rclcpp/rclcpp.hpp>

#include <memory>
#include <utility>

namespace autoware::component_interface_utils_poc
{

namespace specs = autoware::component_interface_specs_poc;

class NodeAdaptor
{
public:
  NodeAdaptor(rclcpp::Node * node, std::shared_ptr<NodeInterface> iface)
  : node_(node), iface_(std::move(iface))
  {
  }

  // create the entity first, then register with its remap-resolved topic name
  // (get_topic_name() returns the FQN after launch-time remapping) so the manifest
  // records both the logical Spec::name (contract identity) and the actual wire topic.
  template <specs::InterfaceSpec S>
  typename rclcpp::Publisher<typename S::Message>::SharedPtr create_publisher()
  {
    auto pub = node_->create_publisher<typename S::Message>(S::name, get_qos<S>());
    iface_->register_provided<S>(pub->get_topic_name());
    return pub;
  }

  template <specs::InterfaceSpec S, class Callback>
  typename rclcpp::Subscription<typename S::Message>::SharedPtr create_subscription(
    Callback && cb, specs::AcceptMajor accept = specs::accept_major(specs::version_of<S>().major))
  {
    auto sub = node_->create_subscription<typename S::Message>(
      S::name, get_qos<S>(), std::forward<Callback>(cb));
    iface_->register_required<S>(accept, sub->get_topic_name());
    return sub;
  }

  template <specs::ServiceSpec S, class Callback>
  typename rclcpp::Service<typename S::Service>::SharedPtr create_service(Callback && cb)
  {
    auto srv = node_->create_service<typename S::Service>(S::name, std::forward<Callback>(cb));
    iface_->register_provided_service<S>(srv->get_service_name());
    return srv;
  }

  template <specs::ServiceSpec S>
  typename rclcpp::Client<typename S::Service>::SharedPtr create_client()
  {
    auto cli = node_->create_client<typename S::Service>(S::name);
    iface_->register_required_service<S>(cli->get_service_name());
    return cli;
  }

private:
  rclcpp::Node * node_;
  std::shared_ptr<NodeInterface> iface_;
};

}  // namespace autoware::component_interface_utils_poc
