# System Architecture Overview

This document describes the high-level architecture and module breakdown of the
`embedded-motor-pid-controller` project.

The goal of this project is to demonstrate a clean, portable PID-based motor
speed controller in embedded C, with a matching Python simulation environment
for tuning and validation.

---

## 1. Project Structure

The project is divided into three main components:

1. **`firmware/`** – the main embedded C application
2. **`sim/`** – the Python simulation and analysis environment
3. **`docs/`** – project documentation (including this file)

```text
embedded-motor-pid-controller/
├─ firmware/
│  ├─ src/
│  │  ├─ main.c
│  │  ├─ motor.c
│  │  ├─ pid.c
│  ├─ include/
│  │  ├─ motor.h
│  │  ├─ pid.h
├─ sim/
│  ├─ pid_simulation.py
├─ docs/
│  ├─ architecture.md
├─ README.md
```

---

## 2. Firmware Architecture (C Modules)

The firmware is designed with **modularity** and **portability** in mind,
separating hardware abstraction (motor control) from the core algorithm
(PID control). This approach makes it easy to:

- Port the code to different microcontrollers / platforms
- Test the PID logic independently of hardware
- Reuse the PID module in other projects

### 2.1 Module Overview

| File(s)        | Module Name                         | Description                                                                                               | Dependencies          |
|----------------|-------------------------------------|-----------------------------------------------------------------------------------------------------------|-----------------------|
| `main.c`       | Application Entry / Control Loop    | System initialization, configuration loading, and main control loop (superloop or RTOS task wrapper).    | `motor`, `pid`        |
| `motor.c/.h`   | Motor Control Abstraction Layer     | Low-level motor interface: configures GPIO/PWM, reads encoder feedback, exposes “set speed/output” API.  | Hardware-specific HAL |
| `pid.c/.h`     | PID Control Algorithm               | Core PID math: error calculation, proportional/integral/derivative terms, output limiting/clamping, etc. | None (pure C)         |

### 2.2 Responsibilities

- **`main.c`**
  - Initializes the system (clock, peripherals, motor, PID configuration).
  - Implements the periodic control loop:
    - Reads the current motor speed/position from `motor`.
    - Calls the `pid` module with setpoint + measured value.
    - Sends the resulting control effort back to the `motor` module.
  - May optionally handle:
    - Simple command interface (e.g., changing setpoint)
    - Fault handling / safe shutdown

- **`motor.c/.h`**
  - Encapsulates all hardware-specific details:
    - PWM channel configuration
    - Timer / encoder setup
    - Reading sensor/encoder counts
  - Provides a **hardware-agnostic API**, for example:
    - `motor_init(...)`
    - `motor_set_output(int16_t duty)`
    - `motor_get_speed(void)`
  - This file is the main porting point when moving to a new MCU.

- **`pid.c/.h`**
  - Contains a PID state struct (e.g. error terms, integral accumulator).
  - Implements:
    - `pid_init(...)`
    - `pid_compute(setpoint, measurement)`
    - Optional helpers (reset, update gains, anti-windup).
  - Designed to be:
    - Reentrant (state passed in via struct)
    - Usable on bare-metal or RTOS-based systems
    - Friendly to fixed-point implementations if needed

---

## 3. Control Loop & Data Flow

The system operates in a **closed-loop** configuration:

- The motor/encoder provides feedback.
- The firmware measures the current speed/position.
- The PID controller computes the control effort based on the setpoint and feedback.
- The motor driver applies this effort via PWM or a similar actuator signal.

```mermaid
graph TD
    A[Physical Motor and Encoder] --> B[Hardware Peripherals: Timers, GPIO, ADC]
    B --> C[Motor Module - motor.c]
    C --> D[Main Control Loop - main.c]
    D --> E[PID Module - pid.c]
    E --> C
```

### 3.1 Flow Description

1. **Input (Feedback):**  
   The motor encoder (or other sensor) provides feedback via hardware
   peripherals (timers, ADC, capture units, etc.).

2. **Abstraction (Motor Module):**  
   The `motor` module reads this feedback and computes a physical quantity
   such as speed (e.g., RPM) or position (e.g., encoder counts).

3. **Control Loop (Application):**  
   The main control loop in `main.c`:
   - Reads the **setpoint** (desired speed/position).
   - Reads the **measurement** from the `motor` module.
   - Calls the `pid` module to compute the control effort.

4. **Processing (PID Module):**  
   The `pid` module calculates the control output using:
   - Proportional term (P)
   - Integral term (I)
   - Derivative term (D)
   and applies any clamping / anti-windup to keep the output in a safe range.

5. **Output (Actuation):**  
   The `motor` module receives the control effort and converts it into
   a physical action (e.g., PWM duty cycle), which drives the motor.

---

## 4. Timing and Execution Model

The control loop can be executed in one of two ways:

1. **Superloop (bare-metal):**
   - `main.c` runs an infinite loop that:
     - Waits for a fixed time slice (e.g., via a timer flag)
     - Executes the control loop at a fixed frequency (e.g., 1 kHz)
   - Simple and works well for basic demos.

2. **RTOS Task (optional):**
   - The PID control logic is run in a periodic RTOS task.
   - Allows separation between:
     - Control tasks
     - Communication / UI tasks
     - Logging / diagnostics

The current project is kept **MCU-agnostic**, so timing details are left to
the porting layer or example implementations.

---

## 5. Simulation Environment

The `sim/pid_simulation.py` file implements a **Python-based simulation** of
the motor + PID loop.

### 5.1 Purpose

- Mirrors the C PID logic to validate:
  - Step response
  - Overshoot
  - Settling time
  - Steady-state error
- Enables **offline tuning** of PID gains (`Kp`, `Ki`, `Kd`) against a
  simple motor model before flashing firmware onto actual hardware.

### 5.2 Tools and Libraries

Typical dependencies (to be listed in the README or requirements file):

- `numpy` – numerical calculations and discrete-time simulation
- `matplotlib` – plotting responses (e.g., setpoint vs. output, error over time)

### 5.3 Workflow

1. Define or load a motor model (e.g., first-order system).
2. Implement a PID controller in Python that matches the C implementation.
3. Run simulations for various:
   - Gains (`Kp`, `Ki`, `Kd`)
   - Step inputs
   - Disturbances (e.g., load changes)
4. Once satisfactory tuning is achieved, transfer the gains into the embedded firmware.

---

## 6. Portability and Extension Points

This architecture is intentionally simple and extendable:

- **Porting to new hardware:**
  - Only `motor.c/.h` and platform-specific startup code need changes.
  - PID core remains the same.

- **Extending the controller:**
  - Support for multiple motors / axes
  - Additional control modes (e.g., position + velocity)
  - Feedforward terms or advanced filters

- **Integrating with larger systems:**
  - Expose a simple API or protocol for higher-level control (e.g., via UART/CAN).
  - Wrap the firmware in an RTOS-based application with communication tasks.

---

_End of file._
