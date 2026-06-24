#include "autoware_interface_admission/admission_rule.hpp"

#include "autoware_component_interface_utils_poc/admission_transport.hpp"

#include <rclcpp/rclcpp.hpp>

#include "autoware_common_msgs_poc/msg/admission_result.hpp"
#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <map>
#include <string>

namespace utils = autoware::component_interface_utils_poc;

using autoware_common_msgs_poc::msg::AdmissionResult;
using autoware_common_msgs_poc::msg::InterfaceManifest;

class AdmissionNode : public rclcpp::Node
{
public:
  AdmissionNode() : Node("autoware_interface_admission")
  {
    // Plain endpoints from the shared transport contract (see admission_transport.hpp):
    // the subscriber retains one-per-publisher, the publisher latches recent verdicts.
    using InVersion = utils::InterfaceVersionTransport;
    using OutResult = utils::AdmissionResultTransport;
    sub_ = create_subscription<InVersion::Message>(
      InVersion::name, utils::admission_transport_qos(InVersion::subscriber_depth),
      [this](InterfaceManifest::SharedPtr m) { on_manifest(m); });
    pub_ = create_publisher<OutResult::Message>(
      OutResult::name, utils::admission_transport_qos(OutResult::publisher_depth));
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
      if (res.code == AdmissionResult::ACCEPTED) {
        RCLCPP_INFO(
          get_logger(), "ADMIT %s <- %s (%s)", res.consumer_node.c_str(),
          res.provider_node.c_str(), res.interface_name.c_str());
      } else {
        RCLCPP_WARN(
          get_logger(), "REJECT %s <- %s (%s): %s [observe-only]", res.consumer_node.c_str(),
          res.provider_node.c_str(), res.interface_name.c_str(),
          autoware::interface_admission::verdict_text(res.code));
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
