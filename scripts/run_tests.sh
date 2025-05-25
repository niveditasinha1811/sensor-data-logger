#!/usr/bin/env bash
#------------------------------------------------------------------------------
# Run all unit tests (host & cross-platform) using CMake/CTest
#------------------------------------------------------------------------------
set -euo pipefail

# Project root is one level above this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/../build"

# Create & enter build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure & build tests
cmake ..
cmake --build . --target test

# Run tests with output on failure
ctest --output-on-failure
