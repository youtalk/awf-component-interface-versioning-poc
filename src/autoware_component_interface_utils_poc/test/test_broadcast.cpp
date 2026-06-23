#include <gtest/gtest.h>

#include "autoware_component_interface_specs_poc/perception.hpp"
#include "autoware_component_interface_utils_poc/node_adaptor.hpp"
#include "autoware_component_interface_utils_poc/node_interface.hpp"

#include <rclcpp/rclcpp.hpp>

#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <chrono>
#include <memory>

namespace specs = autoware::component_interface_specs_poc;
namespace utils = autoware::component_interface_utils_poc;
using autoware_common_msgs_poc::msg::InterfaceManifest;

TEST(Broadcast, manifest_reaches_late_subscriber_via_transient_local)
{
  rclcpp::init(0, nullptr);

  auto provider = std::make_shared<rclcpp::Node>("provider");
  auto iface = std::make_shared<utils::NodeInterface>();
  utils::NodeAdaptor adaptor(provider.get(), iface);
  auto pub = adaptor.create_publisher<specs::perception::ObjectRecognition>();
  iface->broadcast(provider.get(), "autoware");

  // Subscriber created AFTER the broadcast: transient_local must still deliver it.
  auto listener = std::make_shared<rclcpp::Node>("listener");
  InterfaceManifest got;
  bool received = false;
  rclcpp::QoS qos(1);
  qos.reliable().transient_local();
  auto sub = listener->create_subscription<InterfaceManifest>(
    "/system/interface_version", qos,
    [&](InterfaceManifest::SharedPtr m) { got = *m; received = true; });

  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(provider);
  exec.add_node(listener);
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  while (!received && std::chrono::steady_clock::now() < deadline) {
    exec.spin_some();
  }

  EXPECT_TRUE(received);
  ASSERT_EQ(got.provided.size(), 1u);
  EXPECT_EQ(got.provided[0].interface_name, "/perception/object_recognition/objects");
  EXPECT_EQ(got.provided[0].major, 2);

  rclcpp::shutdown();
}
