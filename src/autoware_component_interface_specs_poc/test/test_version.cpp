#include <gtest/gtest.h>
#include "autoware_component_interface_specs_poc/version.hpp"

namespace specs = autoware::component_interface_specs_poc;

TEST(Version, ordering_and_equality)
{
  EXPECT_TRUE((specs::Version{2, 1, 0} == specs::Version{2, 1, 0}));
  EXPECT_TRUE((specs::Version{2, 1, 0} > specs::Version{2, 0, 9}));
  EXPECT_TRUE((specs::Version{1, 9, 9} < specs::Version{2, 0, 0}));
}

TEST(AcceptMajor, contains)
{
  EXPECT_TRUE(specs::accept_major(2).contains(2));
  EXPECT_FALSE(specs::accept_major(2).contains(3));
  EXPECT_FALSE(specs::accept_major(2).contains(1));
  EXPECT_TRUE(specs::accept_major(2, 3).contains(3));
  EXPECT_FALSE(specs::accept_major(2, 3).contains(4));
}
