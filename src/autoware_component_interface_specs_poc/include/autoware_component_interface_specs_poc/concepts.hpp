#pragma once

#include <concepts>
#include <cstddef>

namespace autoware::component_interface_specs_poc
{

template <class T>
concept InterfaceSpec = requires {
  typename T::Message;
  { T::name } -> std::convertible_to<const char *>;
  { T::depth } -> std::convertible_to<std::size_t>;
  T::reliability;
  T::durability;
};

template <class T>
concept ServiceSpec = requires {
  typename T::Service;
  { T::name } -> std::convertible_to<const char *>;
};

}  // namespace autoware::component_interface_specs_poc
