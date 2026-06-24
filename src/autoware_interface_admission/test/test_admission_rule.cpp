#include <gtest/gtest.h>

#include "autoware_interface_admission/admission_rule.hpp"

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

InterfaceManifest provider(uint16_t major, uint16_t minor, const std::string & resolved = kIf)
{
  InterfaceManifest m;
  m.node_name = "/provider";
  ProvidedInterface p;
  p.declared_topic = kIf;
  p.resolved_topic = resolved;
  p.major = major;
  p.minor = minor;
  m.provided.push_back(p);
  return m;
}

InterfaceManifest consumer(
  uint16_t lo, uint16_t hi, uint16_t min_minor = 0, const std::string & resolved = kIf)
{
  InterfaceManifest m;
  m.node_name = "/consumer";
  RequiredInterface r;
  r.declared_topic = kIf;
  r.resolved_topic = resolved;
  r.accept_major_min = lo;
  r.accept_major_max = hi;
  r.min_minor = min_minor;
  m.required.push_back(r);
  return m;
}
}  // namespace

TEST(AdmissionRule, accepts_same_major)
{
  const auto results = adm::evaluate({provider(2, 1), consumer(2, 2)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].code, AdmissionResult::ACCEPTED);
}

TEST(AdmissionRule, rejects_higher_required_major)
{
  const auto results = adm::evaluate({provider(2, 1), consumer(3, 3)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].code, AdmissionResult::MAJOR_MISMATCH);
}

TEST(AdmissionRule, accepts_within_widened_range)
{
  const auto results = adm::evaluate({provider(3, 0), consumer(2, 3)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].code, AdmissionResult::ACCEPTED);
}

TEST(AdmissionRule, rejects_when_min_minor_unmet)
{
  const auto results = adm::evaluate({provider(2, 1), consumer(2, 2, /*min_minor=*/5)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].code, AdmissionResult::MINOR_MISMATCH);
}

TEST(AdmissionRule, rejects_remap_topic_mismatch)
{
  // Same logical IF + compatible MAJOR, but the provider's wire topic was remapped away.
  // Logical-name-only admission (matching on Spec::name alone) would false-accept; the
  // resolved_topic comparison catches the disjoint wiring.
  const auto results = adm::evaluate(
    {provider(2, 1, "/perception/object_recognition/objects_remapped"), consumer(2, 2)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0].code, AdmissionResult::TOPIC_MISMATCH);
}

TEST(AdmissionRule, no_result_without_provider)
{
  const auto results = adm::evaluate({consumer(2, 2)});
  EXPECT_TRUE(results.empty());
}
