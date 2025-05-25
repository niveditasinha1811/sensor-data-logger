# Sensor Data Logger

This repository implements a MISRA-C compliant sensor data logger with:

- A circular buffer of 128 timestamped accelerometer samples (±16 G).
- A mock sensor generator producing random data & epoch timestamps (ms resolution).
- Unity-based unit tests for both modules.
- Static analysis using Cppcheck for MISRA-style checks.
- A management script (`manage.sh`) to streamline build, test, lint, and clean tasks.

## Prerequisites

- **CMake** 3.13+  
- **C compiler** supporting C11 (e.g., GCC, Clang)  
- **Bash** (for helper scripts)  
- **Cppcheck** (for static analysis)

## Directory Structure

```
/                          # project root
├── CMakeLists.txt         # build configuration
├── include/               # public headers
├── src/                   # implementation sources
├── tests/                 # unit tests
├── scripts/               # helper scripts (run_tests.sh, static_analysis.sh, build.sh, manage.sh)
├── README.md              # this file
└── docs/                  # static-analysis reports
```

## Usage

This project includes a single management script that supports the following commands:

```bash
# From project root:
./scripts/manage.sh <command>
```

| Command   | Description                                                  |
|-----------|--------------------------------------------------------------|
| `build`   | Configure & build all targets (clean-first).                |
| `run-test`| Run unit tests via CTest.                                    |
| `lint`    | Run static analysis (Cppcheck) and output to `docs/`.       |
| `clean`   | Remove `build/` directory and static-analysis reports.       |
| `all`     | Alias for `build`, then `run-test`, then `lint`.            |

Example:

```bash
./scripts/manage.sh all
```

This will:
1. Create or reuse `build/`, configure and build with CMake (`clean-first`).
2. Execute all unit tests and show results.
3. Run Cppcheck for MISRA-style checks and store the log in `docs/static_analysis.log`.

---

### Individual Tasks

If you prefer individual steps, you can also use the existing scripts:

```bash
# Configure & build (clean-first)
mkdir -p build && cd build
cmake ..
cmake --build . --clean-first

# Run tests
ctest --output-on-failure

# Static analysis
bash ../scripts/static_analysis.sh

# Clean build artifacts
cmake --build . --target clean-all
```
