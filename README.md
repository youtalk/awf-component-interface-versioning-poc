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
# (--build rebuilds the image with the pinned deps; required on a clean clone)
CONSUMER_MAJOR=2 EXPECT_ACCEPTED=true \
  docker compose --profile test up --build --abort-on-container-exit --exit-code-from test

# Inter-container REJECT: consumer built against MAJOR 3, provider at 2.1.0
CONSUMER_MAJOR=3 EXPECT_ACCEPTED=false \
  docker compose --profile test up --abort-on-container-exit --exit-code-from test

# Live demo (watch verdicts on /system/admission_result)
docker compose up
```

## Deploy-time gate (shift-left: detect before boot)

Bake each component's manifest into its image, then admit the image set **before** any
container starts — catching a mismatch at deploy-config time instead of at runtime.

```bash
# Bake provider + two consumer variants (manifest baked as an OCI label, no app boot)
./bake_image.sh provider 2 civ-poc:provider
./bake_image.sh consumer 2 civ-poc:consumer-major2
./bake_image.sh consumer 3 civ-poc:consumer-major3

# Reject an incompatible set before `docker compose up` (exit 1)
CONSUMER_IMAGE=civ-poc:consumer-major3 ./deploy_check.sh compose.deploy-check.yaml

# Accept a compatible set (exit 0)
CONSUMER_IMAGE=civ-poc:consumer-major2 ./deploy_check.sh compose.deploy-check.yaml
```

The gate reads manifests with `docker inspect` (pure image metadata) and runs the **same**
`evaluate()` admission rule as the runtime handshake. Remap-resolved (`resolved_name`) matching
stays with the runtime path (C6); the deploy-time gate matches on version + interface name.

## Claim → evidence map

| Claim | What it proves | How to see it |
| --- | --- | --- |
| C1 | real-shaped spec structs satisfy the C++20 `concept` non-invasively | `test_concept` (`static_assert`) |
| C2 | per-namespace `Version` recovered from a spec type via ADL, structs untouched | `test_concept` |
| C3 | one manifest auto-derived from the `Specs` tuple + the `create_*` choke point | `test_manifest` + `test_accumulate` |
| C4 | `transient_local` handshake discovered across containers (incl. late join) | inter-container accept + late-join run |
| C5 | MAJOR mismatch rejected across containers → `AdmissionResult.code = MAJOR_MISMATCH` (non-zero) | inter-container reject run |
| C6 | a launch-time remap leaves a version-compatible provider/consumer on disjoint wire topics → admission flags `AdmissionResult.code = TOPIC_MISMATCH` (the false-accept that logical-name-only matching would miss; manifest carries `interface_name` + remap-resolved `resolved_name`) | `test_remap_topic_mismatch` (launch) + `rejects_remap_topic_mismatch` (gtest) |
| C7 | each component image carries its IF manifest as an OCI label (`org.autoware.interface_manifest`) + `/opt/autoware/manifest.json`, retrievable **without starting the container and without source in the image** | `docker inspect -f '{{ index .Config.Labels "org.autoware.interface_manifest" }}' <image>` after `./bake_image.sh` |
| C8 | `manifest_admit` reuses the runtime `evaluate()` over embedded manifests → the C5 `MAJOR_MISMATCH` reproduced statically | `test_manifest_admit` (gtest) + `manifest_admit` exit code |
| C9 | an incompatible image set is rejected **before `docker compose up`** (no ROS / DDS / container start), with the same verdict as the runtime handshake | `./deploy_check.sh compose.deploy-check.yaml` → exit 1 (reject) / 0 (accept) |

## Layout

| Package | Role |
|---|---|
| `autoware_common_msgs_poc` | handshake messages |
| `autoware_component_interface_specs_poc` | `Version`, concept, verbatim specs, ADL version, manifest |
| `autoware_component_interface_utils_poc` | `NodeAdaptor` + manifest broadcast |
| `autoware_interface_admission` | system-level admission checker (observe mode) + manifest_admit (deploy-time gate) |
| `interface_versioning_demo_nodes` | provider / consumer + tests |

## License

Apache-2.0.
