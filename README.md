# Autoware Component Interface Versioning — PoC

Self-contained, Docker-reproducible proof of concept for **per-namespace semantic
versioning of Autoware's component interfaces**: it extends the spec-struct idea
behind `autoware_component_interface_specs` with a `Version`, an auto-derived
machine-readable manifest, and a runtime `transient_local` handshake enforced by
a system-level admission checker. The headline result: a provider and a consumer
in **separate containers** carrying incompatible interface MAJORs are rejected.

## Requirements

- Docker + Docker Compose

## Run

```bash
# Fast tier: gtest (Version, concept + ADL version recovery, manifest, admission
# rule) + single-host launch_pytest (accept + reject)
docker compose run --rm --build checks

# Inter-container ACCEPT: provider + consumer (same MAJOR) + admission, each its
# own container on a unicast CycloneDDS network
CONSUMER_MAJOR=2 EXPECT_ACCEPTED=true \
  docker compose --profile test up --abort-on-container-exit --exit-code-from test

# Inter-container REJECT: consumer built against MAJOR 3, provider at 2.1.0
CONSUMER_MAJOR=3 EXPECT_ACCEPTED=false \
  docker compose --profile test up --abort-on-container-exit --exit-code-from test

# Live demo (watch verdicts on /system/admission_result)
docker compose up
```

## Claim → evidence map

| Claim | What it proves | How to see it |
| --- | --- | --- |
| C1 | real-shaped spec structs satisfy the C++20 `concept` non-invasively | `test_concept` (`static_assert`) |
| C2 | per-namespace `Version` recovered from a spec type via ADL, structs untouched | `test_concept` |
| C3 | one manifest auto-derived from the `Specs` tuple + the `create_*` choke point | `test_manifest` + `test_accumulate` |
| C4 | `transient_local` handshake discovered across containers (incl. late join) | inter-container accept + late-join run |
| C5 | MAJOR mismatch rejected across containers → `AdmissionResult{accepted=false}` | inter-container reject run |

## Layout

| Package | Role |
|---|---|
| `autoware_common_msgs_poc` | handshake messages |
| `autoware_component_interface_specs_poc` | `Version`, concept, verbatim specs, ADL version, manifest |
| `autoware_component_interface_utils_poc` | `NodeAdaptor` + manifest broadcast |
| `autoware_interface_admission` | system-level admission checker (observe mode) |
| `interface_versioning_demo_nodes` | provider / consumer + tests |

## License

Apache-2.0.
