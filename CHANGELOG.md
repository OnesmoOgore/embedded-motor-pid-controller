# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.1.0] – 2025-02-17
### Added
- Initial version of the embedded motor PID controller project
- Modular firmware structure (`main.c`, `motor.c`, `pid.c`)
- Simulation environment (`sim/pid_simulation.py`) with log generation and plotting
- GitHub Actions CI pipeline (Ubuntu + Windows, Python 3.11)
- CI artifacts: `log.csv`, `step_response.png`
- Documentation:
  - `README.md` with features, build instructions, badges, and example plot
  - `docs/architecture.md` detailing system architecture
  - `docs/ci.md` describing CI workflow
- Example step-response image under `docs/images/`

### Improved
- README formatting, folder structure summary, and documentation links
- Added CI, MIT License, and language badges to README

### Future
- Motor model refinement
- Gain sweep automation
- PID unit tests
- GitHub Pages dashboard

---

## [1.0.0] – 2025-11-19
### Added
- **Unit testing infrastructure** with Unity framework (12 comprehensive tests)
- **Professional CMake build system** with configurable options
  - Modular library targets (`pid_controller`, `motor_model`)
  - Debug/Release configurations
  - CTest integration
  - Cross-platform support (Linux, Windows, macOS)
  - Installation rules
- **Advanced PID features**:
  - `pid_init_advanced()` for fine-grained control
  - Proper anti-windup via integrator clamping
  - Derivative-on-measurement (eliminates derivative kick)
  - Optional low-pass filtering for derivative term
- **Documentation**:
  - `docs/BUILD.md` - Comprehensive build instructions
  - `Doxyfile` - API documentation configuration
  - `requirements.txt` - Python dependencies
- **Extended PID structure** with integrator limits and derivative filtering

### Changed
- **CI/CD pipeline** split into parallel test and simulation jobs
- **PID computation** now uses derivative-on-measurement instead of derivative-on-error
- **README** significantly enhanced with features, examples, and build instructions
- **Updated `.gitignore`** for Unity framework and build artifacts

### Improved
- **Anti-windup implementation** - integrator properly clamped to prevent windup
- **Derivative calculation** - immune to setpoint changes, reduces actuator wear
- **Code organization** - modular libraries for easy integration
- **Test coverage** - all major PID functionality verified
- **Build system** - professional CMake replacing hardcoded compilation

### Technical Details
- Integrator limits prevent accumulation beyond useful range during saturation
- Derivative-on-measurement: `d = Kd * -(measurement[n] - measurement[n-1]) / dt`
- Low-pass filter: `derivative_filtered = α * derivative_filtered + (1-α) * derivative_raw`
- Backward compatible: original `pid_init()` still works with sensible defaults
- All 12 unit tests pass on Ubuntu and Windows

### Breaking Changes
- None - fully backward compatible with v0.1.0

---

## [Unreleased]
### Planned
- Code coverage reporting (gcov/lcov)
- Gain sweep automation tools
- More realistic motor dynamics model
- Auto-tuning algorithms (Ziegler-Nichols)
- RTOS integration examples
- Hardware-in-loop testing guide
