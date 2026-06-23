#include <gtest/gtest.h>

#include "autoware_component_interface_specs_poc/manifest_json.hpp"
#include "autoware_component_interface_specs_poc/perception.hpp"
#include "autoware_component_interface_specs_poc/system.hpp"

#include <string>

namespace specs = autoware::component_interface_specs_poc;

TEST(Manifest, emits_perception_entry)
{
  const std::string json = specs::tuple_to_json<specs::perception::Specs>();
  EXPECT_NE(json.find("\"/perception/object_recognition/objects\""), std::string::npos);
  EXPECT_NE(json.find("\"ns\":\"perception\""), std::string::npos);
  EXPECT_NE(json.find("\"major\":2"), std::string::npos);
}

TEST(Manifest, emits_service_entry)
{
  const std::string json = specs::tuple_to_json<specs::system::Specs>();
  EXPECT_NE(json.find("\"/system/reset_diagnostics\""), std::string::npos);
  EXPECT_NE(json.find("\"major\":1"), std::string::npos);
}
