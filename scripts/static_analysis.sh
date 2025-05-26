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
+  --suppress=missingIncludeSystem \
   --output-file="${REPORT_DIR}/static_analysis.log" \
   -I "${PROJECT_ROOT}/include" \
   "${PROJECT_ROOT}/src"


echo "Static analysis complete. See ${REPORT_DIR}/static_analysis.log"
