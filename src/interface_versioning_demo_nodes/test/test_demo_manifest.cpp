#include <gtest/gtest.h>

#include "interface_versioning_demo_nodes/demo_manifest.hpp"

namespace demo = interface_versioning_demo_nodes;

TEST(DemoManifest, provider_provides_object_recognition_at_2_1_0)
{
  const auto m = demo::build_manifest("provider", 0);
  ASSERT_EQ(m.provided.size(), 1u);
  EXPECT_TRUE(m.required.empty());
  EXPECT_EQ(m.provided[0].interface_name, "/perception/object_recognition/objects");
  EXPECT_EQ(m.provided[0].major, 2u);
  EXPECT_EQ(m.provided[0].minor, 1u);
  EXPECT_EQ(m.owner, "autowarefoundation");
}

TEST(DemoManifest, consumer_requires_with_accept_major)
{
  const auto m = demo::build_manifest("consumer", 3);
  ASSERT_EQ(m.required.size(), 1u);
  EXPECT_TRUE(m.provided.empty());
  EXPECT_EQ(m.required[0].interface_name, "/perception/object_recognition/objects");
  EXPECT_EQ(m.required[0].accept_major_min, 3u);
  EXPECT_EQ(m.required[0].accept_major_max, 3u);
}
