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

## [Unreleased]
### Planned
- Configurable simulation parameters
- Python-based PID + plant model for fast tuning
- Real MCU HAL backend example
- Automated gain-sweep CI job
