#include <gtest/gtest.h>

#include "autoware_component_interface_specs_poc/perception.hpp"
#include "autoware_component_interface_utils_poc/node_interface.hpp"

namespace specs = autoware::component_interface_specs_poc;
namespace utils = autoware::component_interface_utils_poc;

TEST(NodeInterface, accumulates_provided_with_namespace_version)
{
  utils::NodeInterface iface;
  iface.register_provided<specs::perception::ObjectRecognition>();

  const auto & m = iface.manifest();
  ASSERT_EQ(m.provided.size(), 1u);
  EXPECT_EQ(m.provided[0].ns, "perception");
  EXPECT_EQ(m.provided[0].declared_topic, "/perception/object_recognition/objects");
  EXPECT_EQ(m.provided[0].major, 2);
  EXPECT_EQ(m.provided[0].minor, 1);
}

TEST(NodeInterface, accumulates_required_with_accept_range)
{
  utils::NodeInterface iface;
  iface.register_required<specs::perception::ObjectRecognition>(specs::accept_major(2));

  const auto & m = iface.manifest();
  ASSERT_EQ(m.required.size(), 1u);
  EXPECT_EQ(m.required[0].accept_major_min, 2);
  EXPECT_EQ(m.required[0].accept_major_max, 2);
}
