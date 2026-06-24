#include "autoware_component_interface_specs_poc/perception.hpp"
#include "autoware_component_interface_utils_poc/node_adaptor.hpp"
#include "autoware_component_interface_utils_poc/node_interface.hpp"

#include <rclcpp/rclcpp.hpp>

#include <autoware_perception_msgs/msg/predicted_objects.hpp>

#include <chrono>
#include <memory>

namespace specs = autoware::component_interface_specs_poc;
namespace utils = autoware::component_interface_utils_poc;

class ProviderNode : public rclcpp::Node
{
public:
  ProviderNode() : Node("provider")
  {
    iface_ = std::make_shared<utils::NodeInterface>();
    utils::NodeAdaptor adaptor(this, iface_);
    pub_ = adaptor.create_publisher<specs::perception::ObjectRecognition>();
    iface_->broadcast(this, "autowarefoundation");
    timer_ = create_wall_timer(
      std::chrono::milliseconds(200), [this]() {
        autoware_perception_msgs::msg::PredictedObjects msg;
        msg.header.stamp = now();
        pub_->publish(msg);
      });
  }

private:
  std::shared_ptr<utils::NodeInterface> iface_;
  rclcpp::Publisher<autoware_perception_msgs::msg::PredictedObjects>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ProviderNode>());
  rclcpp::shutdown();
  return 0;
}
