#pragma once

#include <compare>
#include <cstdint>

namespace autoware::component_interface_specs_poc
{

struct Version
{
  std::uint16_t major{};
  std::uint16_t minor{};
  std::uint16_t patch{};
  constexpr auto operator<=>(const Version &) const = default;
};

struct AcceptMajor
{
  std::uint16_t min{};
  std::uint16_t max{};
  constexpr bool contains(std::uint16_t m) const { return min <= m && m <= max; }
};

constexpr AcceptMajor accept_major(std::uint16_t m) { return AcceptMajor{m, m}; }
constexpr AcceptMajor accept_major(std::uint16_t lo, std::uint16_t hi) { return AcceptMajor{lo, hi}; }

}  // namespace autoware::component_interface_specs_poc
