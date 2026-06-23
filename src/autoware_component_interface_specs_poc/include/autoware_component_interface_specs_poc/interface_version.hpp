#pragma once

#include "autoware_component_interface_specs_poc/version.hpp"

#include <string_view>

namespace autoware::component_interface_specs_poc
{

// version_of<S>() / namespace_of<S>() recover the per-namespace metadata of an
// arbitrary spec type S WITHOUT the struct carrying it: the unqualified calls
// below are dependent on S, so they resolve via ADL at instantiation to the
// get_interface_version / get_interface_namespace defined in S's own namespace.
template <class S>
constexpr Version version_of()
{
  return get_interface_version(S{});
}

template <class S>
constexpr std::string_view namespace_of()
{
  return get_interface_namespace(S{});
}

}  // namespace autoware::component_interface_specs_poc
