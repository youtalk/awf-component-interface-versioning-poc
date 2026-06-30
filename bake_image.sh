#!/usr/bin/env bash
# Build a role image and attach its interface manifest as an OCI label.
#   ./bake_image.sh <provider|consumer> <consumer_major> <image_tag>
# The label is read from the fixed-path manifest the build baked in, extracted via
# `docker create` + `docker cp` (the application is never started).
set -euo pipefail

ROLE="${1:?usage: bake_image.sh <role> <consumer_major> <tag>}"
MAJOR="${2:?usage: bake_image.sh <role> <consumer_major> <tag>}"
TAG="${3:?usage: bake_image.sh <role> <consumer_major> <tag>}"

echo "[bake] building ${TAG} (role=${ROLE} major=${MAJOR})"
docker build --build-arg "ROLE=${ROLE}" --build-arg "CONSUMER_MAJOR=${MAJOR}" -t "${TAG}" .

echo "[bake] extracting baked manifest (no app boot)"
cid="$(docker create "${TAG}")"
trap 'docker rm -f "${cid}" >/dev/null 2>&1 || true' EXIT
manifest="$(docker cp "${cid}:/opt/autoware/manifest.json" - | tar -xO)"

echo "[bake] attaching OCI label org.autoware.interface_manifest"
echo "FROM ${TAG}" | docker build --label "org.autoware.interface_manifest=${manifest}" -t "${TAG}" -

echo "[bake] done: ${TAG}"
