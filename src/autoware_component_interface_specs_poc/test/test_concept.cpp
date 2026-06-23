#include <gtest/gtest.h>

#include "autoware_component_interface_specs_poc/concepts.hpp"
#include "autoware_component_interface_specs_poc/interface_version.hpp"
#include "autoware_component_interface_specs_poc/perception.hpp"
#include "autoware_component_interface_specs_poc/system.hpp"

namespace specs = autoware::component_interface_specs_poc;

// C1: verbatim-shaped structs satisfy the concepts, no struct changes.
static_assert(specs::InterfaceSpec<specs::perception::ObjectRecognition>);
static_assert(specs::ServiceSpec<specs::system::ResetDiagnostics>);

// C2: per-namespace version recovered from the spec type via ADL.
static_assert(specs::version_of<specs::perception::ObjectRecognition>() == specs::Version{2, 1, 0});
static_assert(specs::version_of<specs::system::ResetDiagnostics>() == specs::Version{1, 0, 0});

TEST(Concept, adl_recovers_distinct_namespace_versions)
{
  EXPECT_EQ(specs::version_of<specs::perception::ObjectRecognition>().major, 2);
  EXPECT_EQ(specs::version_of<specs::system::ResetDiagnostics>().major, 1);
  EXPECT_EQ(specs::namespace_of<specs::perception::ObjectRecognition>(), "perception");
  EXPECT_EQ(specs::namespace_of<specs::system::ResetDiagnostics>(), "system");
}
