#pragma once

#include "autoware_interface_admission/admission_rule.hpp"

#include "autoware_common_msgs_poc/msg/admission_result.hpp"
#include "autoware_common_msgs_poc/msg/interface_manifest.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace autoware::interface_admission
{

// Parse one InterfaceManifest JSON (the shape emitted by to_json / manifest_emit).
inline autoware_common_msgs_poc::msg::InterfaceManifest from_json(const std::string & doc)
{
  using nlohmann::json;
  const auto j = json::parse(doc);
  autoware_common_msgs_poc::msg::InterfaceManifest m;
  m.owner = j.value("owner", std::string{});
  m.node_name = j.value("node_name", std::string{});
  for (const auto & p : j.value("provided", json::array())) {
    autoware_common_msgs_poc::msg::ProvidedInterface pi;
    pi.ns = p.value("ns", std::string{});
    pi.interface_name = p.value("interface_name", std::string{});
    pi.resolved_name = p.value("resolved_name", pi.interface_name);
    pi.type_name = p.value("type_name", std::string{});
    pi.major = static_cast<std::uint16_t>(p.value("major", 0));
    pi.minor = static_cast<std::uint16_t>(p.value("minor", 0));
    pi.patch = static_cast<std::uint16_t>(p.value("patch", 0));
    m.provided.push_back(pi);
  }
  for (const auto & r : j.value("required", json::array())) {
    autoware_common_msgs_poc::msg::RequiredInterface ri;
    ri.ns = r.value("ns", std::string{});
    ri.interface_name = r.value("interface_name", std::string{});
    ri.resolved_name = r.value("resolved_name", ri.interface_name);
    ri.type_name = r.value("type_name", std::string{});
    ri.accept_major_min = static_cast<std::uint16_t>(r.value("accept_major_min", 0));
    ri.accept_major_max = static_cast<std::uint16_t>(r.value("accept_major_max", 0));
    ri.min_minor = static_cast<std::uint16_t>(r.value("min_minor", 0));
    m.required.push_back(ri);
  }
  return m;
}

// The deploy-time trigger of the SAME rule: parse N manifests, then run evaluate().
inline std::vector<autoware_common_msgs_poc::msg::AdmissionResult> evaluate_jsons(
  const std::vector<std::string> & docs)
{
  std::vector<autoware_common_msgs_poc::msg::InterfaceManifest> manifests;
  manifests.reserve(docs.size());
  for (const auto & d : docs) {
    manifests.push_back(from_json(d));
  }
  return evaluate(manifests);
}

inline bool any_rejected(
  const std::vector<autoware_common_msgs_poc::msg::AdmissionResult> & results)
{
  for (const auto & r : results) {
    if (r.code != autoware_common_msgs_poc::msg::AdmissionResult::ACCEPTED) {
      return true;
    }
  }
  return false;
}

}  // namespace autoware::interface_admission
