# Build Instructions

This document provides detailed instructions for building the PID controller firmware, running tests, and generating documentation.

## Prerequisites

### Required Tools

- **C Compiler**: GCC, Clang, or MSVC
- **CMake**: Version 3.15 or higher
- **Git**: For cloning dependencies
- **Python 3.11+**: For running simulations (optional)

### Optional Tools

- **Doxygen**: For generating API documentation
- **Graphviz**: For documentation diagrams

## Quick Start

### 1. Clone Unity Test Framework

```bash
git clone https://github.com/ThrowTheSwitch/Unity.git tests/Unity
```

### 2. Configure and Build

```bash
mkdir build
cd build
cmake ..
make
```

### 3. Run Tests

```bash
./test_pid
# Or using CTest
ctest --output-on-failure
```

## Build Options

### Build Types

#### Debug Build (with debugging symbols)
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

#### Release Build (optimized)
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Build Options

- `BUILD_TESTS`: Build unit tests (default: ON)
- `BUILD_DEMO`: Build demo application (default: ON)

Example:
```bash
# Build without tests
cmake -DBUILD_TESTS=OFF ..

# Build only library (no demo or tests)
cmake -DBUILD_TESTS=OFF -DBUILD_DEMO=OFF ..
```

## Build Targets

### All Targets
```bash
make              # Build everything
```

### Individual Targets
```bash
make pid_controller   # Build PID library only
make motor_model      # Build motor model library
make pid_demo         # Build demo application
make test_pid         # Build unit tests
```

### Test Target
```bash
make run_tests    # Build and run all tests
```

## Cross-Compilation

### For ARM Cortex-M (example with arm-none-eabi-gcc)

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake ..
make
```

Create `toolchain-arm.cmake`:
```cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
```

## Running the Demo

```bash
./build/pid_demo
```

The demo runs a simple motor control simulation and prints the results to stdout.

## Running Tests

### Using the test executable directly
```bash
./build/test_pid
```

### Using CTest
```bash
cd build
ctest --output-on-failure
ctest --verbose  # More detailed output
```

## Python Simulation

### Install Python dependencies
```bash
pip install -r requirements.txt
```

### Run simulation
```bash
cd sim
python pid_simulation.py
```

This generates:
- `log.csv`: Time-series data
- `step_response.png`: Response plot

## Generating Documentation

### Generate Doxygen API documentation
```bash
doxygen Doxyfile
```

Output will be in `docs/api/html/index.html`

## Installation

### Install library and headers
```bash
cd build
sudo make install
```

Default installation locations:
- Library: `/usr/local/lib/libpid_controller.a`
- Headers: `/usr/local/include/pid.h`

### Custom installation prefix
```bash
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
make install
```

## Continuous Integration

The project uses GitHub Actions for CI/CD:

- **Unit Tests**: Runs on Ubuntu and Windows
- **Simulation**: Generates step response plots
- **Build Verification**: Ensures code compiles with strict warnings

See `.github/workflows/ci.yml` for details.

## Troubleshooting

### Unity not found
```
Error: Could not find Unity test framework
```
**Solution**: Clone Unity into `tests/Unity`:
```bash
git clone https://github.com/ThrowTheSwitch/Unity.git tests/Unity
```

### CMake version too old
```
CMake Error: CMake 3.15 or higher is required
```
**Solution**: Update CMake or use a newer version:
```bash
pip install cmake --upgrade
```

### Math library linking error (Linux)
```
undefined reference to `sqrt`, `pow`, etc.
```
**Solution**: The CMakeLists.txt should automatically link `-lm` on Unix systems. If not, verify your CMake configuration.

## Platform-Specific Notes

### Windows
- Use Visual Studio CMake support or CMake GUI
- Alternatively, use MinGW or Cygwin for GCC

### macOS
- Install Xcode Command Line Tools: `xcode-select --install`
- Or use Homebrew: `brew install cmake gcc`

### Linux
- Most distributions include all required tools
- Install build essentials: `sudo apt install build-essential cmake`

## Next Steps

- Read [Architecture Documentation](architecture.md)
- Review [API Documentation](api/html/index.html) (after running Doxygen)
- Explore the simulation in `sim/pid_simulation.py`
- Check [CHANGELOG.md](../CHANGELOG.md) for version history
