#pragma once

#include "autoware_common_msgs_poc/msg/admission_result.hpp"
#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace autoware::interface_admission
{

inline std::vector<autoware_common_msgs_poc::msg::AdmissionResult> evaluate(
  const std::vector<autoware_common_msgs_poc::msg::InterfaceManifest> & manifests)
{
  using autoware_common_msgs_poc::msg::AdmissionResult;
  using autoware_common_msgs_poc::msg::ProvidedInterface;

  struct ProviderEntry
  {
    std::string node;
    ProvidedInterface p;
  };
  std::unordered_map<std::string, std::vector<ProviderEntry>> providers;
  for (const auto & m : manifests) {
    for (const auto & p : m.provided) {
      providers[p.interface_name].push_back({m.node_name, p});
    }
  }

  std::vector<AdmissionResult> results;
  for (const auto & m : manifests) {
    for (const auto & r : m.required) {
      const auto it = providers.find(r.interface_name);
      if (it == providers.end() || it->second.empty()) {
        continue;  // no provider yet — nothing to admit
      }

      const ProviderEntry * wired = nullptr;          // version-ok AND same resolved wire topic
      const ProviderEntry * version_ok_other = nullptr;  // version-ok but disjoint wire topic
      for (const auto & entry : it->second) {
        const bool major_ok =
          r.accept_major_min <= entry.p.major && entry.p.major <= r.accept_major_max;
        const bool minor_ok = (r.min_minor == 0) || (entry.p.minor >= r.min_minor);
        if (major_ok && minor_ok) {
          if (entry.p.resolved_name == r.resolved_name) {
            wired = &entry;
            break;
          }
          if (version_ok_other == nullptr) {
            version_ok_other = &entry;
          }
        }
      }

      AdmissionResult res;
      res.consumer_node = m.node_name;
      res.interface_name = r.interface_name;
      if (wired != nullptr) {
        res.code = AdmissionResult::ACCEPTED;
        res.provider_node = wired->node;
      } else if (version_ok_other != nullptr) {
        // version-compatible, but a remap left the wire topics disjoint — the false-accept
        // that logical-name-only admission (matching on Spec::name alone) would have missed.
        res.code = AdmissionResult::TOPIC_MISMATCH;
        res.provider_node = version_ok_other->node;
      } else {
        const auto & first = it->second.front();
        res.provider_node = first.node;
        const bool major_in_range =
          r.accept_major_min <= first.p.major && first.p.major <= r.accept_major_max;
        res.code =
          major_in_range ? AdmissionResult::MINOR_MISMATCH : AdmissionResult::MAJOR_MISMATCH;
      }
      results.push_back(res);
    }
  }
  return results;
}

// The human-readable reason is derived from the verdict code off-wire — it is not
// carried on AdmissionResult (the code is the single source of identity, mirroring
// the unified-code approach of the Error Code Foundation).
inline const char * verdict_text(std::uint16_t code)
{
  using autoware_common_msgs_poc::msg::AdmissionResult;
  switch (code) {
    case AdmissionResult::ACCEPTED:
      return "accepted";
    case AdmissionResult::MAJOR_MISMATCH:
      return "MAJOR mismatch";
    case AdmissionResult::MINOR_MISMATCH:
      return "MINOR mismatch";
    case AdmissionResult::TOPIC_MISMATCH:
      return "resolved-topic mismatch (remap)";
    default:
      return "unknown";
  }
}

}  // namespace autoware::interface_admission
