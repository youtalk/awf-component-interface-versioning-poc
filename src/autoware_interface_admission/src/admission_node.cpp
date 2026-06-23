#include "autoware_interface_admission/admission_rule.hpp"

#include <rclcpp/rclcpp.hpp>

#include "autoware_common_msgs_poc/msg/admission_result.hpp"
#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <map>
#include <string>

using autoware_common_msgs_poc::msg::AdmissionResult;
using autoware_common_msgs_poc::msg::InterfaceManifest;

class AdmissionNode : public rclcpp::Node
{
public:
  AdmissionNode() : Node("autoware_interface_admission")
  {
    rclcpp::QoS in_qos(100);
    in_qos.reliable().transient_local();
    sub_ = create_subscription<InterfaceManifest>(
      "/system/interface_version", in_qos,
      [this](InterfaceManifest::SharedPtr m) { on_manifest(m); });

    rclcpp::QoS out_qos(10);
    out_qos.reliable().transient_local();
    pub_ = create_publisher<AdmissionResult>("/system/admission_result", out_qos);
  }

private:
  void on_manifest(const InterfaceManifest::SharedPtr m)
  {
    manifests_[m->node_name] = *m;

    std::vector<InterfaceManifest> all;
    all.reserve(manifests_.size());
    for (const auto & [name, manifest] : manifests_) {
      all.push_back(manifest);
    }

    for (const auto & res : autoware::interface_admission::evaluate(all)) {
      pub_->publish(res);
      if (res.accepted) {
        RCLCPP_INFO(
          get_logger(), "ADMIT %s <- %s (%s)", res.consumer_node.c_str(),
          res.provider_node.c_str(), res.interface_name.c_str());
      } else {
        RCLCPP_WARN(
          get_logger(), "REJECT %s <- %s (%s): %s [observe-only]", res.consumer_node.c_str(),
          res.provider_node.c_str(), res.interface_name.c_str(), res.reason.c_str());
      }
    }
  }

  std::map<std::string, InterfaceManifest> manifests_;
  rclcpp::Subscription<InterfaceManifest>::SharedPtr sub_;
  rclcpp::Publisher<AdmissionResult>::SharedPtr pub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AdmissionNode>());
  rclcpp::shutdown();
  return 0;
}
