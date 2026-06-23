#pragma once

#include "autoware_common_msgs_poc/msg/admission_result.hpp"
#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

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

      const ProviderEntry * match = nullptr;
      for (const auto & entry : it->second) {
        const bool major_ok =
          r.accept_major_min <= entry.p.major && entry.p.major <= r.accept_major_max;
        const bool minor_ok = (r.min_minor == 0) || (entry.p.minor >= r.min_minor);
        if (major_ok && minor_ok) {
          match = &entry;
          break;
        }
      }

      AdmissionResult res;
      res.consumer_node = m.node_name;
      res.interface_name = r.interface_name;
      if (match != nullptr) {
        res.accepted = true;
        res.error_code = 0;
        res.provider_node = match->node;
        res.reason = "compatible";
      } else {
        const auto & first = it->second.front();
        res.accepted = false;
        res.error_code = 1;
        res.provider_node = first.node;
        const bool major_in_range = r.accept_major_min <= first.p.major &&
                                    first.p.major <= r.accept_major_max;
        if (!major_in_range) {
          res.reason = "MAJOR mismatch: required " + std::to_string(r.accept_major_min) + ".." +
                       std::to_string(r.accept_major_max) + ", provided " +
                       std::to_string(first.p.major);
        } else {
          res.reason = "MINOR mismatch: required >=" + std::to_string(r.min_minor) +
                       ", provided " + std::to_string(first.p.minor);
        }
      }
      results.push_back(res);
    }
  }
  return results;
}

}  // namespace autoware::interface_admission
