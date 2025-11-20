# Build Instructions

This document provides comprehensive instructions for building, testing, and integrating the **Embedded Motor PID Controller** project.

**Target Audience**: Developers, integrators, and engineers who want to build and use the PID controller library.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Quick Start](#quick-start)
3. [Build Configuration](#build-configuration)
4. [Build Targets](#build-targets)
5. [Running Tests](#running-tests)
6. [Python Simulation](#python-simulation)
7. [Cross-Compilation](#cross-compilation)
8. [IDE Integration](#ide-integration)
9. [Library Integration](#library-integration)
10. [Documentation Generation](#documentation-generation)
11. [Installation](#installation)
12. [Troubleshooting](#troubleshooting)
13. [Platform-Specific Notes](#platform-specific-notes)

---

## Prerequisites

### Required Tools

| Tool | Minimum Version | Purpose |
|------|----------------|---------|
| **C Compiler** | GCC 7+ / Clang 6+ / MSVC 2017+ | Compile C99 code |
| **CMake** | 3.15+ | Build system |
| **Git** | Any recent | Clone dependencies |

### Optional Tools

| Tool | Purpose |
|------|---------|
| **Python 3.11+** | Run simulations and plotting |
| **Doxygen** | Generate API documentation |
| **Graphviz** | Generate call graphs in docs |
| **Make/Ninja** | Build acceleration (auto-detected) |

### Installing Prerequisites

**Ubuntu/Debian**:
```bash
sudo apt update
sudo apt install build-essential cmake git python3 python3-pip
# Optional
sudo apt install doxygen graphviz
```

**macOS**:
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Or use Homebrew
brew install cmake git python3
# Optional
brew install doxygen graphviz
```

**Windows**:
- Install [Visual Studio 2019+](https://visualstudio.microsoft.com/) with C++ tools
- Install [CMake](https://cmake.org/download/)
- Install [Git for Windows](https://git-scm.com/download/win)
- Install [Python 3.11+](https://www.python.org/downloads/)
- Or use [MSYS2](https://www.msys2.org/) / [MinGW](http://mingw-w64.org/)

---

## Quick Start

### 1. Clone the Repository

```bash
git clone https://github.com/OnesmoOgore/embedded-motor-pid-controller.git
cd embedded-motor-pid-controller
```

### 2. Clone Unity Test Framework

```bash
git clone https://github.com/ThrowTheSwitch/Unity.git tests/Unity
```

**Why Unity?** Industry-standard C testing framework for embedded systems, used by thousands of projects.

### 3. Configure and Build

```bash
mkdir build && cd build
cmake ..
make
```

**Expected Output**:
```
-- The C compiler identification is GNU 13.3.0
-- Configuring done (0.7s)
-- Build files have been written to: .../build
[ 10%] Building C object CMakeFiles/pid_controller.dir/firmware/src/pid.c.o
...
[100%] Built target test_pid
```

### 4. Run Tests

```bash
./test_pid
```

**Expected Output**:
```
tests/test_pid.c:207:test_pid_init_sets_parameters:PASS
tests/test_pid.c:208:test_pid_proportional_only:PASS
...
-----------------------
12 Tests 0 Failures 0 Ignored
OK
```

### 5. Run Demo (Optional)

```bash
./pid_demo
```

This runs a simple motor control simulation with console output.

---

## Build Configuration

### Build Types

CMake supports two primary build types:

#### Debug Build (Development)
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

**Features**:
- Debug symbols enabled (`-g`)
- No optimization (`-O0`)
- Easier debugging with GDB/LLDB
- Larger binary size
- **Use for**: Development, debugging, testing

#### Release Build (Production)
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

**Features**:
- Optimizations enabled (`-O2`)
- No debug symbols
- Smaller, faster binary
- Assertions disabled (`-DNDEBUG`)
- **Use for**: Production deployment, performance testing

### Build Options

Control what gets built with CMake options:

```bash
# Disable unit tests
cmake -DBUILD_TESTS=OFF ..

# Disable demo application
cmake -DBUILD_DEMO=OFF ..

# Build only the PID library (minimal build)
cmake -DBUILD_TESTS=OFF -DBUILD_DEMO=OFF ..

# Combine with build type
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF ..
```

### Compiler Warnings

The project uses strict warning flags by default:
```
-Wall -Wextra -Werror
```

This means **all warnings are treated as errors**. This is intentional to maintain code quality.

To build with warnings as warnings (not recommended):
```bash
# Edit CMakeLists.txt and remove -Werror
```

---

## Build Targets

### Overview of Targets

The CMake build system creates several targets:

| Target | Type | Description |
|--------|------|-------------|
| `pid_controller` | Static Library | Core PID implementation |
| `motor_model` | Static Library | Simple motor plant model |
| `pid_demo` | Executable | Demo application |
| `test_pid` | Executable | Unit tests |
| `unity` | Static Library | Unity test framework |

### Building Specific Targets

```bash
# Build everything (default)
make

# Build only PID library
make pid_controller

# Build only motor model
make motor_model

# Build demo application
make pid_demo

# Build unit tests
make test_pid

# Build and run tests
make run_tests
```

### Using Ninja (Faster Builds)

```bash
cmake -G Ninja ..
ninja
```

Ninja is typically 20-30% faster than Make for incremental builds.

### Parallel Builds

```bash
# Use all CPU cores
make -j$(nproc)           # Linux
make -j$(sysctl -n hw.ncpu)  # macOS
make -j%NUMBER_OF_PROCESSORS%  # Windows

# Or specify cores manually
make -j4
```

---

## Running Tests

### Test Execution Methods

#### Method 1: Direct Execution
```bash
./build/test_pid
```

**Output**:
```
tests/test_pid.c:207:test_pid_init_sets_parameters:PASS
tests/test_pid.c:208:test_pid_proportional_only:PASS
tests/test_pid.c:209:test_pid_integral_only:PASS
tests/test_pid.c:210:test_pid_derivative_only:PASS
tests/test_pid.c:211:test_pid_output_clamp_max:PASS
tests/test_pid.c:212:test_pid_output_clamp_min:PASS
tests/test_pid.c:213:test_pid_combined_terms:PASS
tests/test_pid.c:214:test_pid_reset:PASS
tests/test_pid.c:215:test_pid_zero_gains:PASS
tests/test_pid.c:216:test_pid_negative_error:PASS
tests/test_pid.c:217:test_pid_derivative_kick:PASS
tests/test_pid.c:218:test_pid_integral_accumulation:PASS

-----------------------
12 Tests 0 Failures 0 Ignored
OK
```

#### Method 2: CTest
```bash
cd build
ctest --output-on-failure
```

**Benefits**:
- Standardized test execution
- Integration with CI/CD
- XML/JSON output for reporting
- Parallel test execution

#### Method 3: CTest with Options
```bash
# Verbose output
ctest --verbose

# Run specific test
ctest -R PID_Tests

# Stop on first failure
ctest --stop-on-failure

# Parallel execution
ctest -j4
```

### Test Coverage Analysis (Advanced)

```bash
# Build with coverage flags
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="--coverage" ..
make

# Run tests
./test_pid

# Generate coverage report
gcov ../tests/test_pid.c
# Or use lcov for HTML report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

---

## Python Simulation

### Installation

```bash
# Install Python dependencies
pip install -r requirements.txt
```

**Dependencies**:
- `numpy>=1.24.0,<2.0.0` - Numerical calculations
- `matplotlib>=3.7.0,<4.0.0` - Plotting

### Running the Simulation

```bash
cd sim
python pid_simulation.py
```

### Simulation Workflow

1. **Compilation**: Compiles firmware with GCC
2. **Execution**: Runs PID control loop with motor model
3. **Data Logging**: Saves results to `log.csv`
4. **Visualization**: Generates `step_response.png`
5. **Display**: Shows plot (GUI mode) or saves only (CI mode)

### Simulation Outputs

**`log.csv`** format:
```csv
time,setpoint,measurement,output
0.000,100.0,0.0,0.0
0.010,100.0,0.5,95.0
0.020,100.0,1.8,88.2
...
```

**`step_response.png`**: Visual plot showing:
- Setpoint (reference)
- Measured value (process variable)
- Rise time, overshoot, settling time

### Tuning PID Gains

Edit `firmware/src/main.c` to adjust gains:
```c
pid_init(&motor_pid,
         1.0f,  // Kp - increase for faster response
         0.5f,  // Ki - increase to eliminate steady-state error
         0.1f,  // Kd - increase to reduce overshoot
         0.01f, // dt - sample time (fixed)
         -100.0f, 100.0f);  // Output limits
```

**Note**: The example gains shown above (1.0/0.5/0.1) differ from the firmware defaults (0.8/0.3/0.05) to demonstrate how tuning affects system response. The firmware uses conservatively tuned values that provide stable performance with minimal overshoot.

Rebuild and re-run simulation to see effects.

---
## Cross-Compilation

### Concept

Cross-compilation allows you to build binaries for a different platform than your development machine (e.g., build ARM binaries on x86 Linux).

### ARM Cortex-M Example (STM32, Nordic, etc.)

**1. Create Toolchain File** (`toolchain-arm.cmake`):

```cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Specify the cross compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# Compiler flags for Cortex-M4
set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# For libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

**2. Build**:
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake -DBUILD_TESTS=OFF ..
make
```

**Output**: `pid_demo.elf` suitable for flashing to ARM Cortex-M4 MCU

### ESP32 Example

```cmake
# toolchain-esp32.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER xtensa-esp32-elf-gcc)

set(CMAKE_C_FLAGS_INIT "-mlongcalls")
```

### Raspberry Pi Pico (RP2040) Example

```bash
# Use Pico SDK
export PICO_SDK_PATH=/path/to/pico-sdk
cmake -DPICO_BOARD=pico ..
make
```

### Integration with MCU Projects

**For STM32CubeIDE:**
```cmake
# Add to existing STM32 CMakeLists.txt
add_subdirectory(path/to/pid-controller)
target_link_libraries(your_project PRIVATE pid_controller)
```

**For ESP-IDF:**
```cmake
# components/pid/CMakeLists.txt
idf_component_register(
    SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../../firmware/src/pid.c"
    INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../firmware/include"
)
```

---

## IDE Integration

### Visual Studio Code

**1. Install Extensions**:
- C/C++ (Microsoft)
- CMake Tools (Microsoft)
- CMake (twxs)

**2. Configure** (`.vscode/settings.json`):
```json
{
  "cmake.configureOnOpen": true,
  "cmake.buildDirectory": "${workspaceFolder}/build",
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

**3. Build**:
- Press `Ctrl+Shift+P` → "CMake: Build"
- Or click "Build" in status bar

**4. Debug**:
- Set breakpoints in source
- Press `F5` to start debugging

### CLion (JetBrains)

**1. Open Project**:
- File → Open → Select root directory
- CLion auto-detects CMakeLists.txt

**2. Configure Build**:
- File → Settings → Build, Execution, Deployment → CMake
- Add configurations (Debug/Release)

**3. Build and Run**:
- Build → Build Project (`Ctrl+F9`)
- Run → Run 'test_pid' (`Shift+F10`)

### Visual Studio (Windows)

**1. Open Folder**:
- File → Open → Folder → Select project root

**2. CMake Auto-Configuration**:
- VS 2019+ detects CMakeLists.txt automatically

**3. Select Target**:
- Use dropdown to select `test_pid.exe` or `pid_demo.exe`

**4. Build**:
- Build → Build All (`Ctrl+Shift+B`)

---

## Library Integration

### Using in Your Own Project

#### Option 1: CMake Subdirectory (Recommended)

**Your Project Structure**:
```
your_project/
├── CMakeLists.txt
├── src/
│   └── main.c
└── external/
    └── pid-controller/  # Git submodule or copy
```

**Your CMakeLists.txt**:
```cmake
cmake_minimum_required(VERSION 3.15)
project(your_project C)

# Add PID controller as subdirectory
add_subdirectory(external/pid-controller EXCLUDE_FROM_ALL)

add_executable(your_app src/main.c)

# Link PID library
target_link_libraries(your_app PRIVATE pid_controller)

# PID headers automatically available via target_link_libraries
```

#### Option 2: Installed Library

**Install PID library**:
```bash
cd pid-controller/build
sudo make install
```

**Your CMakeLists.txt**:
```cmake
# Find installed PID library
find_library(PID_LIB pid_controller)
find_path(PID_INCLUDE pid.h)

add_executable(your_app src/main.c)
target_link_libraries(your_app PRIVATE ${PID_LIB})
target_include_directories(your_app PRIVATE ${PID_INCLUDE})
```

### Example Integration Code

```c
// main.c
#include "pid.h"
#include <stdio.h>

int main(void) {
    // Create PID instance
    pid_t temperature_controller;

    // Initialize: Kp=2.0, Ki=0.5, Kd=0.1, dt=100ms
    pid_init(&temperature_controller,
             2.0f, 0.5f, 0.1f, 0.1f,
             0.0f, 100.0f);  // PWM output 0-100%

    // Control loop
    float setpoint = 75.0f;  // Target: 75°C
    while (1) {
        float temperature = read_sensor();

        float pwm_duty = pid_compute(&temperature_controller,
                                     setpoint,
                                     temperature);

        set_heater_pwm(pwm_duty);
        delay_ms(100);  // Match dt parameter
    }

    return 0;
}
```

---

## Documentation Generation

### Doxygen API Documentation

**Generate Documentation**:
```bash
doxygen Doxyfile
```

**View Documentation**:
```bash
# Linux
xdg-open docs/api/html/index.html

# macOS
open docs/api/html/index.html

# Windows
start docs/api/html/index.html
```

**Documentation Includes**:
- Main Page & Modules
- Data Structures (`pid_t`)
- Function Documentation
- Call Graphs (requires Graphviz)

---

## Installation

### System-Wide Installation

```bash
cd build
sudo make install
```

**Installed**:
- `/usr/local/lib/libpid_controller.a`
- `/usr/local/include/pid.h`

### Custom Prefix

```bash
cmake -DCMAKE_INSTALL_PREFIX=$HOME/local ..
make install
```

---

## Troubleshooting

### Unity not found
```
Error: Could not find Unity test framework
```
**Solution**:
```bash
git clone https://github.com/ThrowTheSwitch/Unity.git tests/Unity
```

### CMake version too old
**Solution**:
```bash
pip install cmake --upgrade
```

### Linking errors (Linux)
**Solution**: CMakeLists.txt automatically links `-lm`. Verify CMake configuration.

### Build fails on Windows
**Solution**: Use Visual Studio 2019+ or install MinGW-w64

---

## Platform-Specific Notes

### Linux (Ubuntu/Debian)
```bash
sudo apt install build-essential cmake git
```

### macOS
```bash
brew install cmake
```

### Windows
- Visual Studio 2019+ with C++ tools
- Or MSYS2/MinGW

---

## Next Steps

- [Architecture Documentation](architecture.md) - System design & CI/CD pipeline
- [CHANGELOG](../CHANGELOG.md) - Version history
- [API Documentation](api/html/index.html) - After running Doxygen

---

_**For Support**: Report issues at [GitHub Issues](https://github.com/OnesmoOgore/embedded-motor-pid-controller/issues)_
