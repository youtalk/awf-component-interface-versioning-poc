#include "autoware_interface_admission/admission_rule.hpp"
#include "autoware_interface_admission/manifest_parse.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Deploy-time admission gate: read N InterfaceManifest JSON files (one per component image,
// extracted from image metadata before boot), run the SAME evaluate() the runtime handshake
// uses, print the verdicts, and exit non-zero if any required interface is not ACCEPTED.
int main(int argc, char ** argv)
{
  if (argc < 2) {
    std::cerr << "usage: manifest_admit <manifest.json> [<manifest.json> ...]\n";
    return 2;
  }

  std::vector<std::string> docs;
  for (int i = 1; i < argc; ++i) {
    std::ifstream f(argv[i]);
    if (!f) {
      std::cerr << "manifest_admit: cannot open " << argv[i] << "\n";
      return 2;
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    docs.push_back(ss.str());
  }

  std::vector<autoware_common_msgs_poc::msg::AdmissionResult> results;
  try {
    results = autoware::interface_admission::evaluate_jsons(docs);
  } catch (const std::exception & e) {
    std::cerr << "manifest_admit: failed to parse manifest: " << e.what() << "\n";
    return 2;
  }
  for (const auto & r : results) {
    std::cout << r.consumer_node << " <- " << r.provider_node << " [" << r.interface_name
              << "]: " << autoware::interface_admission::deploy_verdict_text(r.code) << " (code=" << r.code
              << ")\n";
  }
  if (results.empty()) {
    std::cout << "manifest_admit: no consumer/provider pairings to evaluate\n";
  }
  return autoware::interface_admission::any_rejected(results) ? 1 : 0;
}
