#!/usr/bin/env bash
# Deploy-time interface admission gate.
# Given a compose file, extract each image's baked interface manifest via `docker inspect`
# (no container is created or started), then run the SAME admission rule the runtime handshake
# uses (manifest_admit). Exit non-zero on a mismatch — BEFORE `docker compose up`.
#
# Images under test need only carry the org.autoware.interface_manifest label (so a binary-only
# third-party image works); the admission binary itself runs from a dedicated tool image,
# ADMIT_TOOL_IMAGE (default: this repo's image), NEVER from an image under test.
#   ./deploy_check.sh compose.deploy-check.yaml
#   ADMIT_TOOL_IMAGE=my/tool:tag ./deploy_check.sh compose.deploy-check.yaml
set -euo pipefail

COMPOSE_FILE="${1:?usage: deploy_check.sh <compose-file>}"
LABEL="org.autoware.interface_manifest"
ADMIT_TOOL_IMAGE="${ADMIT_TOOL_IMAGE:-awf-component-interface-versioning-poc:latest}"
workdir="$(mktemp -d)"
trap 'rm -rf "${workdir}"' EXIT

# Resolve the image set the deploy config selects.
mapfile -t images < <(docker compose -f "${COMPOSE_FILE}" config --images | sort -u)
if [ "${#images[@]}" -eq 0 ]; then
  echo "deploy_check: no images in ${COMPOSE_FILE}" >&2
  exit 2
fi

i=0
for img in "${images[@]}"; do
  echo "[deploy_check] inspecting ${img}"
  manifest="$(docker inspect -f "{{ index .Config.Labels \"${LABEL}\" }}" "${img}" 2>/dev/null || true)"
  if [ -z "${manifest}" ]; then
    echo "deploy_check: image ${img} has no ${LABEL} label — not IF-versioning conformant" >&2
    exit 2
  fi
  printf '%s' "${manifest}" > "${workdir}/manifest_${i}.json"
  i=$((i + 1))
done

echo "[deploy_check] running admission over ${i} manifest(s) via ${ADMIT_TOOL_IMAGE} (no boot, no DDS)"
docker run --rm -v "${workdir}:/in:ro" "${ADMIT_TOOL_IMAGE}" \
  bash -lc 'source /opt/ros/jazzy/setup.bash && source install/setup.bash &&
            ros2 run autoware_interface_admission manifest_admit /in/manifest_*.json'
