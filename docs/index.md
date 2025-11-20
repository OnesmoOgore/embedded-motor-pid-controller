# Embedded Motor PID Controller

> **Production-ready PID controller implementation for embedded systems**
> Featuring comprehensive testing, professional build system, and advanced control algorithms

[![CI Status](https://github.com/OnesmoOgore/embedded-motor-pid-controller/actions/workflows/ci.yml/badge.svg)](https://github.com/OnesmoOgore/embedded-motor-pid-controller/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/OnesmoOgore/embedded-motor-pid-controller/blob/main/LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-green.svg)](https://github.com/OnesmoOgore/embedded-motor-pid-controller/blob/main/CHANGELOG.md)

---

## 🎯 Quick Navigation

**Choose your path based on your role:**

### 👔 For Recruiters & Hiring Managers
- [**Project Overview**](#overview) - What this demonstrates
- [**Technical Stack**](#technical-stack) - Technologies used
- [**Key Features**](#features) - Production-ready capabilities
- [**Code Quality**](#code-quality) - Testing & standards

### 💻 For Engineers & Developers
- [**Quick Start Guide**](BUILD.md) - Get up and running
- [**API Documentation**](#api-documentation) - Code reference
- [**Architecture**](architecture.md) - System design
- [**Source Code**](https://github.com/OnesmoOgore/embedded-motor-pid-controller) - GitHub repository

### 📊 For Technical Leadership
- [**Production Readiness**](#production-readiness) - Quality metrics
- [**CI/CD Pipeline**](ci.md) - Automated workflows
- [**Version History**](https://github.com/OnesmoOgore/embedded-motor-pid-controller/blob/main/CHANGELOG.md) - Release notes

---

## <a name="overview"></a>📖 Overview

A **professional-grade PID controller** implementation in embedded C, designed for motor control applications. This project demonstrates industry best practices in embedded systems development, including comprehensive testing, modular architecture, and cross-platform CI/CD.

### What is a PID Controller?

A **Proportional-Integral-Derivative (PID) controller** is a control loop mechanism that calculates an error value as the difference between a desired setpoint and a measured process variable. It's widely used in industrial control systems, robotics, and automation.

### Project Highlights

- ✅ **12 comprehensive unit tests** with 90%+ code coverage
- ✅ **Production-ready algorithms** (anti-windup, derivative filtering)
- ✅ **Cross-platform builds** (Linux, Windows, macOS)
- ✅ **Professional CMake build system**
- ✅ **Automated CI/CD** on multiple platforms
- ✅ **Complete documentation** with examples

---

## <a name="features"></a>✨ Key Features

### Advanced PID Algorithms

| Feature | Description | Benefit |
|---------|-------------|---------|
| **Anti-Windup** | Integrator clamping prevents accumulation during saturation | Faster recovery, stable control |
| **Derivative-on-Measurement** | Eliminates derivative kick on setpoint changes | Smoother response, reduced actuator wear |
| **Low-Pass Filtering** | Optional derivative noise filtering | Robust to measurement noise |
| **Configurable Limits** | Independent output and integrator limits | Fine-grained control tuning |

### Software Engineering Excellence

- **Modular Design**: Reusable PID library, easy MCU integration
- **Comprehensive Testing**: Unity framework with edge case coverage
- **Professional Build System**: CMake with Debug/Release configs
- **Documentation**: Doxygen API docs, architecture diagrams, build guides
- **Backward Compatible**: v1.0.0 maintains compatibility with v0.1.0

---

## <a name="technical-stack"></a>🔧 Technical Stack

### Languages & Tools
- **C99** - Core implementation (embedded-friendly)
- **Python 3.11** - Simulation and visualization
- **CMake 3.15+** - Build system
- **Unity** - C unit testing framework
- **Doxygen** - API documentation generation
- **GitHub Actions** - CI/CD automation

### Key Libraries
- `numpy` / `matplotlib` - Data analysis and plotting
- `Unity` - Embedded C testing framework

### Development Standards
- Strict compiler warnings (`-Wall -Wextra -Werror`)
- Semantic versioning (v1.0.0)
- Keep a Changelog format
- MIT License (permissive, commercial-friendly)

---

## <a name="code-quality"></a>✅ Code Quality & Testing

### Test Coverage
```
Total Tests: 12
Pass Rate: 100%
Coverage: ~90%+ of critical paths
```

### Test Categories
- ✅ Initialization & configuration
- ✅ Individual P/I/D term verification
- ✅ Combined PID operation
- ✅ Output clamping & saturation
- ✅ Anti-windup behavior
- ✅ Reset functionality
- ✅ Edge cases (zero gains, negative errors)

### Continuous Integration

Automated testing on **every commit**:
- **Platforms**: Ubuntu, Windows
- **Build Verification**: Strict warning checks
- **Unit Tests**: CTest execution
- **Simulation**: Step response generation
- **Artifacts**: CSV logs, response plots

[View CI Pipeline Details →](ci.md)

---

## <a name="production-readiness"></a>🚀 Production Readiness

### Suitability

This implementation is suitable for:
- ✅ Commercial embedded products
- ✅ Robotics and automation
- ✅ Motor control systems
- ✅ Educational projects
- ✅ Open-source integration

### Quality Indicators

| Metric | Status |
|--------|--------|
| Unit Tests | ✅ Comprehensive (12 tests) |
| Build System | ✅ Professional (CMake) |
| Cross-Platform | ✅ Linux, Windows, macOS |
| Documentation | ✅ Complete (API + guides) |
| CI/CD | ✅ Automated testing |
| Code Style | ✅ Strict warnings enabled |
| License | ✅ MIT (permissive) |

---

## <a name="api-documentation"></a>📚 Documentation

### Core Documentation

| Document | Description | Audience |
|----------|-------------|----------|
| [**README**](https://github.com/OnesmoOgore/embedded-motor-pid-controller#readme) | Quick start & overview | Everyone |
| [**BUILD.md**](BUILD.md) | Comprehensive build guide | Developers |
| [**CHANGELOG.md**](https://github.com/OnesmoOgore/embedded-motor-pid-controller/blob/main/CHANGELOG.md) | Version history | Everyone |
| [**Architecture**](architecture.md) | System design | Engineers |
| [**CI/CD Details**](ci.md) | Pipeline documentation | DevOps/Engineers |

### API Documentation

**Doxygen-generated API reference** (recommended):

```bash
# Generate API documentation
doxygen Doxyfile

# View in browser
open docs/api/html/index.html
```

The API docs include:
- Function signatures and parameters
- Data structure definitions
- Usage examples
- Call graphs and dependencies

---

## 🏁 Quick Start

### For Users

```bash
# 1. Clone repository
git clone https://github.com/OnesmoOgore/embedded-motor-pid-controller.git
cd embedded-motor-pid-controller

# 2. Install dependencies
git clone https://github.com/ThrowTheSwitch/Unity.git tests/Unity
pip install -r requirements.txt

# 3. Build and test
mkdir build && cd build
cmake ..
make
./test_pid

# 4. Run simulation
cd ../sim
python pid_simulation.py
```

[Full Build Instructions →](BUILD.md)

### For Integrators

```c
#include "pid.h"

// Initialize PID controller
pid_t motor_controller;
pid_init(&motor_controller,
         1.0f,      // Kp - Proportional gain
         0.5f,      // Ki - Integral gain
         0.1f,      // Kd - Derivative gain
         0.01f,     // dt - Sample time (10ms)
         -100.0f,   // Output minimum
         100.0f);   // Output maximum

// In control loop (called every 10ms)
float control_output = pid_compute(&motor_controller,
                                   setpoint,
                                   measured_value);
```

---

## 📊 Example Output

### Step Response Simulation

The Python simulation generates a step response plot showing the PID controller's performance:

![Step Response](images/step_response_example.png)

**Simulation outputs:**
- `log.csv` - Time-series data (time, setpoint, measurement, output)
- `step_response.png` - Visualization plot

---

## 🔄 Version History

### Current Release: v1.0.0 (Nov 2025)

Major release with production-ready features:
- Unit testing infrastructure (12 tests)
- Professional CMake build system
- Advanced PID features (anti-windup, derivative filtering)
- Comprehensive documentation
- Enhanced CI/CD pipeline

[View Full Changelog →](https://github.com/OnesmoOgore/embedded-motor-pid-controller/blob/main/CHANGELOG.md)

---

## 🤝 Contributing

Contributions are welcome! This project follows best practices:
- Write unit tests for new features
- Maintain strict compiler warnings
- Follow existing code style
- Update documentation

[GitHub Repository →](https://github.com/OnesmoOgore/embedded-motor-pid-controller)

---

## 📞 Contact & Links

- **GitHub**: [OnesmoOgore/embedded-motor-pid-controller](https://github.com/OnesmoOgore/embedded-motor-pid-controller)
- **Issues**: [Report a bug or request a feature](https://github.com/OnesmoOgore/embedded-motor-pid-controller/issues)
- **License**: [MIT License](https://github.com/OnesmoOgore/embedded-motor-pid-controller/blob/main/LICENSE)

---

## 🙏 Acknowledgments

- **Unity Testing Framework** by ThrowTheSwitch
- PID control theory and best practices from the embedded controls community

---

<p align="center">
  <strong>Built with ❤️ for the embedded systems community</strong>
</p>

<p align="center">
  <a href="https://github.com/OnesmoOgore/embedded-motor-pid-controller">⭐ Star this project on GitHub</a>
</p>
