#pragma once

#include "autoware_component_interface_specs_poc/version.hpp"

#include <std_srvs/srv/trigger.hpp>

#include <string_view>
#include <tuple>

namespace autoware::component_interface_specs_poc::system
{

static constexpr Version version{1, 0, 0};

struct ResetDiagnostics
{
  using Service = std_srvs::srv::Trigger;
  static constexpr char name[] = "/system/reset_diagnostics";
};

template <class S>
constexpr Version get_interface_version(const S &) { return version; }
template <class S>
constexpr std::string_view get_interface_namespace(const S &) { return "system"; }

using Specs = std::tuple<ResetDiagnostics>;

}  // namespace autoware::component_interface_specs_poc::system
