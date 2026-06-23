#include <gtest/gtest.h>
#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"
#include "autoware_common_msgs_poc/msg/admission_result.hpp"

TEST(Messages, manifest_holds_provided_and_required)
{
  autoware_common_msgs_poc::msg::InterfaceManifest m;
  m.owner = "autoware";
  m.node_name = "/provider";
  autoware_common_msgs_poc::msg::ProvidedInterface p;
  p.interface_name = "/perception/object_recognition/objects";
  p.major = 2;
  m.provided.push_back(p);
  autoware_common_msgs_poc::msg::RequiredInterface r;
  r.accept_major_min = 2;
  r.accept_major_max = 2;
  m.required.push_back(r);

  EXPECT_EQ(m.provided.size(), 1u);
  EXPECT_EQ(m.provided[0].major, 2);
  EXPECT_EQ(m.required[0].accept_major_min, 2);
}

TEST(Messages, admission_result_fields)
{
  autoware_common_msgs_poc::msg::AdmissionResult a;
  a.accepted = false;
  a.error_code = 1;
  a.reason = "MAJOR mismatch";
  EXPECT_FALSE(a.accepted);
  EXPECT_EQ(a.error_code, 1);
}
