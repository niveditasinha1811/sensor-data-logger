#!/usr/bin/env bash
#
# scripts/manage.sh
# -----------------
# A little “task runner” for build / test / lint / clean.
#

set -euo pipefail

# Compute project root & build directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "${SCRIPT_DIR}")"
BUILD_DIR="${PROJECT_ROOT}/build"
REPORT_DIR="${PROJECT_ROOT}/docs"

usage() {
  cat <<EOF
Usage: $0 <command>

Commands:
  all         Build, run tests, and lint
  build       Configure & build (clean-first)
  run-test    Execute unit tests
  lint        Run static analysis (Cppcheck)
  clean       Remove build/, reports, and binaries
EOF
  exit 1
}

cmd="${1:-}"
case "$cmd" in
  all)
    "$0" build
    "$0" run-test
    "$0" lint
    ;;

  build)
    echo "==> Configuring & building (clean-first)..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake "$PROJECT_ROOT"
    cmake --build . --clean-first
    ;;

  run-test)
    echo "==> Running unit tests..."
    if [[ ! -d "$BUILD_DIR" ]]; then
      echo "Build directory not found; please run '$0 build' first." >&2
      exit 1
    fi
    cd "$BUILD_DIR"
    # Old command: ctest --output-on-failure
    # New command:
    cmake --build . --target run-tests
    ;;

  lint)
    echo "==> Running static analysis..."
    bash "$SCRIPT_DIR/static_analysis.sh"
    ;;

  clean)
    echo "==> Cleaning all build artifacts and reports..."
    rm -rf "$BUILD_DIR"
    rm -f "$REPORT_DIR/static_analysis.log"
    # rm -f "$REPORT_DIR/misra_report.xlsx"  # if you check in an Excel report
    echo "Clean complete."
    ;;

  *)
    usage
    ;;
esac