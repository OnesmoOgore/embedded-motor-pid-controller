# Embedded Motor PID Controller

Small embedded C project showing a basic PID speed controller for a DC motor.

## Features

- Fixed-point friendly PID implementation
- Simple motor abstraction layer
- Tunable gains
- Python script to simulate and plot step response

## Build

This repo is MCU-agnostic. You can:

- Build for a specific microcontroller by adding your HAL / BSP in `firmware/src/`,
- Or build as a desktop simulation (e.g., with `gcc`) by stubbing hardware functions.

## Folder structure

- `firmware/src` — main loop, motor & PID modules
- `firmware/include` — headers
- `sim/` — Python simulation/plotting
- `docs/` — design notes, block diagrams
