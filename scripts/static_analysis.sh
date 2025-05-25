#!/usr/bin/env bash
#------------------------------------------------------------------------------
# Run MISRA-style static analysis via Cppcheck
#------------------------------------------------------------------------------

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPORT_DIR="${PROJECT_ROOT}/docs"
mkdir -p "${REPORT_DIR}"

cppcheck \
  --enable=all,warning,style,performance,portability,information \
  --inconclusive \
  --std=c11 --language=c \
  --template="{file}:{line}:{severity}:{message}" \
  --output-file="${REPORT_DIR}/static_analysis.log" \
  "${PROJECT_ROOT}/src" "${PROJECT_ROOT}/include"

echo "Static analysis complete. See ${REPORT_DIR}/static_analysis.log"
