#pragma once

#include "autoware_component_interface_specs_poc/concepts.hpp"
#include "autoware_component_interface_specs_poc/interface_version.hpp"

#include <rosidl_runtime_cpp/traits.hpp>

#include <sstream>
#include <string>
#include <tuple>

namespace autoware::component_interface_specs_poc
{

// type_name_of: concept-overloaded so both message and service specs resolve.
template <InterfaceSpec S>
inline std::string type_name_of()
{
  return rosidl_generator_traits::data_type<typename S::Message>();
}
template <ServiceSpec S>
inline std::string type_name_of()
{
  return rosidl_generator_traits::data_type<typename S::Service>();
}

template <class S>
inline std::string spec_to_json()
{
  const Version v = version_of<S>();
  std::ostringstream os;
  os << "{\"ns\":\"" << namespace_of<S>() << "\""
     << ",\"name\":\"" << S::name << "\""
     << ",\"type\":\"" << type_name_of<S>() << "\""
     << ",\"major\":" << v.major
     << ",\"minor\":" << v.minor
     << ",\"patch\":" << v.patch << "}";
  return os.str();
}

template <class Tuple>
inline std::string tuple_to_json()
{
  std::ostringstream os;
  os << "[";
  bool first = true;
  std::apply(
    [&](auto... spec_instances) {
      ((os << (first ? "" : ",") << spec_to_json<decltype(spec_instances)>(), first = false), ...);
    },
    Tuple{});
  os << "]";
  return os.str();
}

}  // namespace autoware::component_interface_specs_poc
