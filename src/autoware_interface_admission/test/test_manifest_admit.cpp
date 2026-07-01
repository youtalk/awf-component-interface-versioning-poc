#include <gtest/gtest.h>

#include "autoware_interface_admission/manifest_parse.hpp"
#include "autoware_interface_admission/manifest_serialize.hpp"

#include "autoware_common_msgs_poc/msg/admission_result.hpp"
#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <string>
#include <vector>

using autoware_common_msgs_poc::msg::AdmissionResult;
using autoware_common_msgs_poc::msg::InterfaceManifest;
using autoware_common_msgs_poc::msg::ProvidedInterface;
using autoware_common_msgs_poc::msg::RequiredInterface;
namespace adm = autoware::interface_admission;

namespace
{
constexpr char kIf[] = "/perception/object_recognition/objects";

std::string provider_json(std::uint16_t major, std::uint16_t minor)
{
  InterfaceManifest m;
  m.owner = "autowarefoundation";
  m.node_name = "/provider";
  ProvidedInterface p;
  p.ns = "perception";
  p.interface_name = kIf;
  p.resolved_name = kIf;
  p.type_name = "autoware_perception_msgs/msg/PredictedObjects";
  p.major = major;
  p.minor = minor;
  p.patch = 0;
  m.provided.push_back(p);
  return adm::to_json(m);
}

std::string consumer_json(std::uint16_t accept_major)
{
  InterfaceManifest m;
  m.owner = "autowarefoundation";
  m.node_name = "/consumer";
  RequiredInterface r;
  r.ns = "perception";
  r.interface_name = kIf;
  r.resolved_name = kIf;
  r.type_name = "autoware_perception_msgs/msg/PredictedObjects";
  r.accept_major_min = accept_major;
  r.accept_major_max = accept_major;
  r.min_minor = 0;
  m.required.push_back(r);
  return adm::to_json(m);
}
}  // namespace

TEST(ManifestAdmit, round_trip_preserves_fields)
{
  const auto m = adm::from_json(provider_json(2, 1));
  ASSERT_EQ(m.provided.size(), 1u);
  EXPECT_EQ(m.provided[0].interface_name, kIf);
  EXPECT_EQ(m.provided[0].major, 2u);
  EXPECT_EQ(m.provided[0].minor, 1u);
}

TEST(ManifestAdmit, accepts_compatible_image_set)
{
  const auto results = adm::evaluate_jsons({provider_json(2, 1), consumer_json(2)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].code, AdmissionResult::ACCEPTED);
  EXPECT_FALSE(adm::any_rejected(results));
}

TEST(ManifestAdmit, rejects_incompatible_image_set)
{
  // Provider 2.1.0, consumer built against MAJOR 3 — the C5 reject, now from JSON manifests.
  const auto results = adm::evaluate_jsons({provider_json(2, 1), consumer_json(3)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].code, AdmissionResult::MAJOR_MISMATCH);
  EXPECT_TRUE(adm::any_rejected(results));
}

TEST(ManifestAdmit, rejects_required_with_no_provider)
{
  // A deploy set where the consumer requires the interface but NO image provides it. The runtime
  // observe-mode evaluate() skips this (a provider may not have started); the deploy-time gate
  // must reject it because the whole set is known up front.
  const auto results = adm::evaluate_jsons({consumer_json(2)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].code, AdmissionResult::NO_PROVIDER);
  EXPECT_TRUE(adm::any_rejected(results));
}
