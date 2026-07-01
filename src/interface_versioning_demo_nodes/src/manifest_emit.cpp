#include "interface_versioning_demo_nodes/demo_manifest.hpp"

#include "autoware_interface_admission/manifest_serialize.hpp"

#include <charconv>
#include <cstdint>
#include <iostream>
#include <string>

namespace
{
constexpr char kUsage[] = "usage: manifest_emit <provider|consumer> [--major <uint16>]\n";

// Runs during image builds (see Dockerfile), so bad input must fail clean with a diagnostic and
// exit 2 rather than throwing (std::stoi) or silently emitting the wrong manifest.
int usage_error(const std::string & detail)
{
  std::cerr << "manifest_emit: " << detail << "\n" << kUsage;
  return 2;
}
}  // namespace

// Emit the demo component's InterfaceManifest as JSON, for baking into the image at build time.
//   manifest_emit provider
//   manifest_emit consumer --major 3
int main(int argc, char ** argv)
{
  if (argc < 2) {
    return usage_error("missing role argument");
  }
  const std::string role = argv[1];
  if (role != "provider" && role != "consumer") {
    return usage_error("unknown role '" + role + "' (expected 'provider' or 'consumer')");
  }

  std::uint16_t consumer_major = 2;
  for (int i = 2; i < argc; ++i) {
    const std::string a = argv[i];
    if (a == "--major") {
      if (i + 1 >= argc) {
        return usage_error("--major requires a value");
      }
      const std::string v = argv[++i];
      unsigned long parsed = 0;
      const auto [ptr, ec] = std::from_chars(v.data(), v.data() + v.size(), parsed);
      if (ec != std::errc{} || ptr != v.data() + v.size() || parsed > UINT16_MAX) {
        return usage_error("invalid --major value '" + v + "' (expected 0..65535)");
      }
      consumer_major = static_cast<std::uint16_t>(parsed);
    } else {
      return usage_error("unknown argument '" + a + "'");
    }
  }

  const auto manifest = interface_versioning_demo_nodes::build_manifest(role, consumer_major);
  std::cout << autoware::interface_admission::to_json(manifest) << std::endl;
  return 0;
}
