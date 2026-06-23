#include "autoware_component_interface_specs_poc/manifest_json.hpp"
#include "autoware_component_interface_specs_poc/perception.hpp"
#include "autoware_component_interface_specs_poc/system.hpp"

#include <iostream>
#include <string>

namespace specs = autoware::component_interface_specs_poc;

// Strip the surrounding '[' and ']' from a tuple_to_json() result.
static std::string inner(const std::string & json)
{
  // json is "[...]" — return everything between the brackets.
  return json.substr(1, json.size() - 2);
}

int main()
{
  std::cout << "{\"owner\":\"" << specs::owner << "\",\"interfaces\":["
            << inner(specs::tuple_to_json<specs::perception::Specs>())
            << ","
            << inner(specs::tuple_to_json<specs::system::Specs>())
            << "]}" << std::endl;
  return 0;
}
