#include <gtest/gtest.h>

#include "autoware_component_interface_utils_poc/admission_transport.hpp"

#include <rclcpp/qos.hpp>

#include "autoware_common_msgs_poc/msg/admission_result.hpp"
#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <rmw/types.h>

#include <string>
#include <type_traits>

namespace utils = autoware::component_interface_utils_poc;

// The transport contract is the single source of truth for the admission
// meta-protocol topics: both the broadcasting side (NodeInterface) and the
// admission node derive their endpoints from these definitions.
TEST(AdmissionTransport, interface_version_contract_is_single_source_of_truth)
{
  EXPECT_EQ(std::string(utils::InterfaceVersionTransport::name), "/system/interface_version");
  // Publisher latches one manifest per node; the aggregating subscriber retains
  // one-per-publisher, hence the deliberate depth asymmetry.
  EXPECT_EQ(utils::InterfaceVersionTransport::publisher_depth, 1u);
  EXPECT_EQ(utils::InterfaceVersionTransport::subscriber_depth, 100u);
  static_assert(
    std::is_same_v<
      utils::InterfaceVersionTransport::Message,
      autoware_common_msgs_poc::msg::InterfaceManifest>);
}

TEST(AdmissionTransport, admission_result_contract_is_single_source_of_truth)
{
  EXPECT_EQ(std::string(utils::AdmissionResultTransport::name), "/system/admission_result");
  EXPECT_EQ(utils::AdmissionResultTransport::publisher_depth, 10u);
  static_assert(
    std::is_same_v<
      utils::AdmissionResultTransport::Message,
      autoware_common_msgs_poc::msg::AdmissionResult>);
}

// Every transport topic shares one QoS policy: reliable + transient_local so a
// late-joining node still observes the latest sample. Depth is per-endpoint.
TEST(AdmissionTransport, qos_policy_is_reliable_transient_local_with_given_depth)
{
  const auto profile = utils::admission_transport_qos(7).get_rmw_qos_profile();
  EXPECT_EQ(profile.reliability, RMW_QOS_POLICY_RELIABILITY_RELIABLE);
  EXPECT_EQ(profile.durability, RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL);
  EXPECT_EQ(profile.depth, 7u);
}
