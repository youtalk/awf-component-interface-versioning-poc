#include <gtest/gtest.h>

#include "autoware_interface_admission/admission_rule.hpp"

#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <vector>

using autoware_common_msgs_poc::msg::InterfaceManifest;
using autoware_common_msgs_poc::msg::ProvidedInterface;
using autoware_common_msgs_poc::msg::RequiredInterface;
namespace adm = autoware::interface_admission;

namespace
{
constexpr char kIf[] = "/perception/object_recognition/objects";

InterfaceManifest provider(uint16_t major, uint16_t minor)
{
  InterfaceManifest m;
  m.node_name = "/provider";
  ProvidedInterface p;
  p.interface_name = kIf;
  p.major = major;
  p.minor = minor;
  m.provided.push_back(p);
  return m;
}

InterfaceManifest consumer(uint16_t lo, uint16_t hi, uint16_t min_minor = 0)
{
  InterfaceManifest m;
  m.node_name = "/consumer";
  RequiredInterface r;
  r.interface_name = kIf;
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
  EXPECT_TRUE(results[0].accepted);
  EXPECT_EQ(results[0].error_code, 0);
}

TEST(AdmissionRule, rejects_higher_required_major)
{
  const auto results = adm::evaluate({provider(2, 1), consumer(3, 3)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_FALSE(results[0].accepted);
  EXPECT_NE(results[0].error_code, 0);
  EXPECT_NE(results[0].reason.find("MAJOR mismatch"), std::string::npos);
}

TEST(AdmissionRule, accepts_within_widened_range)
{
  const auto results = adm::evaluate({provider(3, 0), consumer(2, 3)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_TRUE(results[0].accepted);
}

TEST(AdmissionRule, rejects_when_min_minor_unmet)
{
  const auto results = adm::evaluate({provider(2, 1), consumer(2, 2, /*min_minor=*/5)});
  ASSERT_EQ(results.size(), 1u);
  EXPECT_FALSE(results[0].accepted);
}

TEST(AdmissionRule, no_result_without_provider)
{
  const auto results = adm::evaluate({consumer(2, 2)});
  EXPECT_TRUE(results.empty());
}
