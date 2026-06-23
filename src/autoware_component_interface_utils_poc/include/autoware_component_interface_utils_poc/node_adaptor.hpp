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

  template <specs::InterfaceSpec S>
  typename rclcpp::Publisher<typename S::Message>::SharedPtr create_publisher()
  {
    iface_->register_provided<S>();
    return node_->create_publisher<typename S::Message>(S::name, get_qos<S>());
  }

  template <specs::InterfaceSpec S, class Callback>
  typename rclcpp::Subscription<typename S::Message>::SharedPtr create_subscription(
    Callback && cb, specs::AcceptMajor accept = specs::accept_major(specs::version_of<S>().major))
  {
    iface_->register_required<S>(accept);
    return node_->create_subscription<typename S::Message>(
      S::name, get_qos<S>(), std::forward<Callback>(cb));
  }

  template <specs::ServiceSpec S, class Callback>
  typename rclcpp::Service<typename S::Service>::SharedPtr create_service(Callback && cb)
  {
    iface_->register_provided_service<S>();
    return node_->create_service<typename S::Service>(S::name, std::forward<Callback>(cb));
  }

  template <specs::ServiceSpec S>
  typename rclcpp::Client<typename S::Service>::SharedPtr create_client()
  {
    iface_->register_required_service<S>();
    return node_->create_client<typename S::Service>(S::name);
  }

private:
  rclcpp::Node * node_;
  std::shared_ptr<NodeInterface> iface_;
};

}  // namespace autoware::component_interface_utils_poc
