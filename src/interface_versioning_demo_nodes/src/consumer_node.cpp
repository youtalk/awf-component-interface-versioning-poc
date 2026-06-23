#include "autoware_component_interface_specs_poc/perception.hpp"
#include "autoware_component_interface_utils_poc/node_adaptor.hpp"
#include "autoware_component_interface_utils_poc/node_interface.hpp"

#include <rclcpp/rclcpp.hpp>

#include <autoware_perception_msgs/msg/predicted_objects.hpp>

#include <cstdint>
#include <memory>

namespace specs = autoware::component_interface_specs_poc;
namespace utils = autoware::component_interface_utils_poc;

class ConsumerNode : public rclcpp::Node
{
public:
  ConsumerNode() : Node("consumer")
  {
    const int consumer_major = declare_parameter<int>("consumer_major", 2);
    iface_ = std::make_shared<utils::NodeInterface>();
    utils::NodeAdaptor adaptor(this, iface_);
    sub_ = adaptor.create_subscription<specs::perception::ObjectRecognition>(
      [this](autoware_perception_msgs::msg::PredictedObjects::SharedPtr) {
        if (++count_ % 25 == 0) {
          RCLCPP_INFO(get_logger(), "received %u messages", count_);
        }
      },
      specs::accept_major(static_cast<std::uint16_t>(consumer_major)));
    iface_->broadcast(this, "autoware");
    RCLCPP_INFO(get_logger(), "consumer requires perception accept_major(%d)", consumer_major);
  }

private:
  std::shared_ptr<utils::NodeInterface> iface_;
  rclcpp::Subscription<autoware_perception_msgs::msg::PredictedObjects>::SharedPtr sub_;
  std::uint32_t count_{0};
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ConsumerNode>());
  rclcpp::shutdown();
  return 0;
}
