#!/usr/bin/env bash
set -euo pipefail

# gfxplusplus_Install.sh
# Configure + build + install gfxplusplus (Linux/macOS). From repo root.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT_DIR}/build-install"

# Defaults (override via env vars):
: "${BUILD_TYPE:=Release}"
: "${INSTALL_PREFIX:=${HOME}/.local}"
: "${GENERATOR:=}"
: "${JOBS:=}"
: "${CLEAN:=0}"

if [[ "${CLEAN}" == "1" ]]; then
  echo "[INSTALL] Cleaning build dir: ${BUILD_DIR}"
  rm -rf "${BUILD_DIR}"
fi

mkdir -p "${BUILD_DIR}"

echo "[INSTALL] Configuring (BUILD_TYPE=${BUILD_TYPE})"
cmake_args=(
  "-S" "${ROOT_DIR}"
  "-B" "${BUILD_DIR}"
  "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
  "-DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}"
)

if [[ -n "${GENERATOR}" ]]; then
  cmake_args+=("-G" "${GENERATOR}")
fi

cmake "${cmake_args[@]}"

echo "[INSTALL] Building"
build_args=( "--build" "${BUILD_DIR}" )
if [[ -n "${JOBS}" ]]; then
  build_args+=( "-j" "${JOBS}" )
fi
cmake "${build_args[@]}"

echo "[INSTALL] Installing to: ${INSTALL_PREFIX}"
cmake --install "${BUILD_DIR}"

echo "[INSTALL] Done."
echo "[INSTALL] Tip: ensure ${INSTALL_PREFIX}/lib/cmake/gfxplusplus is in your CMAKE_PREFIX_PATH if another project does find_package(gfxplusplus)."
