#include "interface_versioning_demo_nodes/demo_manifest.hpp"

#include "autoware_interface_admission/manifest_serialize.hpp"

#include <cstdint>
#include <iostream>
#include <string>

// Emit the demo component's InterfaceManifest as JSON, for baking into the image at build time.
//   manifest_emit provider
//   manifest_emit consumer --major 3
int main(int argc, char ** argv)
{
  std::string role = (argc > 1) ? argv[1] : "provider";
  std::uint16_t consumer_major = 2;
  for (int i = 2; i < argc; ++i) {
    const std::string a = argv[i];
    if (a == "--major" && i + 1 < argc) {
      consumer_major = static_cast<std::uint16_t>(std::stoi(argv[++i]));
    }
  }
  const auto manifest = interface_versioning_demo_nodes::build_manifest(role, consumer_major);
  std::cout << autoware::interface_admission::to_json(manifest) << std::endl;
  return 0;
}
