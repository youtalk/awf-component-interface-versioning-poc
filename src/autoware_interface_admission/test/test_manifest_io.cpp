#include <gtest/gtest.h>

#include "autoware_interface_admission/manifest_serialize.hpp"

#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

using autoware_common_msgs_poc::msg::InterfaceManifest;
using autoware_common_msgs_poc::msg::ProvidedInterface;

namespace
{
InterfaceManifest provider_manifest()
{
  InterfaceManifest m;
  m.owner = "autowarefoundation";
  m.node_name = "/provider";
  ProvidedInterface p;
  p.ns = "perception";
  p.interface_name = "/perception/object_recognition/objects";
  p.resolved_name = "/perception/object_recognition/objects";
  p.type_name = "autoware_perception_msgs/msg/PredictedObjects";
  p.major = 2;
  p.minor = 1;
  p.patch = 0;
  m.provided.push_back(p);
  return m;
}
}  // namespace

TEST(ManifestSerialize, emits_expected_fields)
{
  const auto json = autoware::interface_admission::to_json(provider_manifest());
  EXPECT_NE(json.find("\"owner\":\"autowarefoundation\""), std::string::npos);
  EXPECT_NE(json.find("\"interface_name\":\"/perception/object_recognition/objects\""), std::string::npos);
  EXPECT_NE(json.find("\"major\":2"), std::string::npos);
  EXPECT_NE(json.find("\"required\":[]"), std::string::npos);
}
