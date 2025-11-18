# Embedded Motor PID Controller
[![CI](https://github.com/OnesmoOgore/embedded-motor-pid-controller/actions/workflows/ci.yml/badge.svg)](https://github.com/OnesmoOgore/embedded-motor-pid-controller/actions)
 ![License](https://img.shields.io/badge/license-MIT-blue.svg)
 ![Language](https://img.shields.io/badge/language-C%20%20&%20Python-brightgreen) ![Coverage](https://img.shields.io/badge/coverage-coming--soon-lightgrey)


A modular embedded C and Python project demonstrating a PID speed controller for a DC motor.  
Includes simulation tools, data logging, automated plotting, and a full multi-platform CI pipeline.

---

## 🔍 Overview

This project showcases:

- Clean embedded C architecture (hardware abstraction layer + PID control logic)
- A reusable PID module suitable for MCU or desktop builds
- A Python simulation tool for offline tuning and step-response analysis
- Automated CI on multiple platforms (via GitHub Actions)
- Strict compilation rules (`-Wall -Wextra -Werror`) to enforce code quality
- Downloadable CI artifacts (`log.csv`, `step_response.png`) for every build

Designed for embedded, robotics, and controls-focused engineers who value structure, repeatability, and portability.

---

## ✨ Features

- Fixed-point friendly PID implementation  
- Simple motor plant model for desktop testing  
- Tunable PID gains  
- CSV data logging and visualization  
- Cross-platform firmware builds (Linux, macOS, Windows)  
- Portable codebase—MCU-agnostic structure

---

## 📂 Folder Structure

- `firmware/src/` — main control loop, motor, and PID modules  
- `firmware/include/` — header files  
- `sim/` — Python simulation & plotting (`pid_simulation.py`)  
- `docs/` — architecture diagrams, CI documentation  
- `.github/workflows/` — GitHub Actions CI workflow configuration  
- `README.md` – Project overview, build instructions, and status

---

## Build

This repo is MCU-agnostic. You can:
- Build for a specific microcontroller by adding your HAL / BSP in `firmware/src/`,
- Or build as a desktop simulation, see step below.

---

## 🧠 Running the Simulation (Desktop)

Bash shell command:
```bash
cd sim
python pid_simulation.py
```
This will:
1. Compile the firmware using gcc
2. Run the closed-loop simulation
3. Generate log.csv
4. Produce step_response.png
5. Use GUI plotting locally, or a safe non-GUI backend in CI

---

## 📊 Example Step Response

Here is an example step response produced by `sim/pid_simulation.py`:

![Step response](docs/images/step_response_example.png)

---

## 📈 Continuous Integration (CI)

GitHub Actions automatically runs the full toolchain on each push and pull request:
- OS: Ubuntu-latest, Windows-latest
- Python versions: 3.11
Each CI job:
- Compiles firmware with strict warnings-as-errors
- Runs the Python simulation script
- Uploads log.csv and step_response.png as build artifacts
- Fails the build if any warnings or errors occur
You can browse previous runs and download artifacts in the Actions tab.

---

## 🔧 Status

**Current status:** Feature-complete demo
- Firmware builds cleanly
- Simulation + plotting is fully functional
- Multi-platform CI verified
- Ready for enhancements:
  - Gain sweep automation
  - More realistic motor model
  - PID unit tests
  - GitHub Pages dashboard

---

## 📄 License

This project is licensed under the MIT License.
See [`LICENSE`](LICENSE) for details.

---

## 📚 Documentation

- [Architecture Overview](docs/architecture.md)
- [CI Workflow Details](docs/ci.md)
