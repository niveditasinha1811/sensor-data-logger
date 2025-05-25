#!/usr/bin/env bash
set -euo pipefail

# Resolve project root (one directory above this script)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "${SCRIPT_DIR}")"
BUILD_DIR="${PROJECT_ROOT}/build"

# Create (or reuse) the build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# 1) Configure
cmake "${PROJECT_ROOT}"

# 2) Build (clean first)
cmake --build . --clean-first

# 3) Run unit tests via CTest
ctest --output-on-failure
