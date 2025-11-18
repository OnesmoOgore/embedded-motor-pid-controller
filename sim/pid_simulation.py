#-------------------------------------------------------------------------------
# @file    pid_simulation.py
# @author  Onesmo Ogore
# @date    11/17/2025
# @brief   A Python script to simulate the PID controller behavior offline.
#
# SPDX-License-Identifier: MIT
#
# Build and run the embedded PID demo, capture its output into log.csv,
# then plot setpoint, speed, and control output.
#
# Assumes:
# - firmware/src/main.c, motor.c, pid.c
# - firmware/include/ contains the headers
#
# This helps validate the control loop logic before deployment to embedded hardware.
#!/usr/bin/env python3
#-------------------------------------------------------------------------------

import subprocess
import sys
from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt


# Paths
ROOT = Path(__file__).resolve().parents[1]   # repo root
FIRMWARE_SRC = ROOT / "firmware" / "src"
FIRMWARE_INC = ROOT / "firmware" / "include"
BUILD_DIR = ROOT / "build"
LOG_FILE = ROOT / "sim" / "log.csv"

# Name of the compiled executable (Windows vs Unix)
EXE_NAME = "pid_demo.exe" if sys.platform.startswith("win") else "pid_demo"
EXE_PATH = BUILD_DIR / EXE_NAME


# Compile the C sources into a small desktop executable.
def build_firmware():
    BUILD_DIR.mkdir(exist_ok=True)

    cmd = [
        "gcc",
        f"-I{FIRMWARE_INC}",
        str(FIRMWARE_SRC / "main.c"),
        str(FIRMWARE_SRC / "pid.c"),
        str(FIRMWARE_SRC / "motor.c"),
        "-o",
        str(EXE_PATH),
    ]

    print("Building firmware:")
    print(" ", " ".join(cmd))
    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
        print("❌ Build failed:")
        print(result.stdout)
        print(result.stderr)
        raise SystemExit(result.returncode)

    print("✅ Build succeeded:", EXE_PATH)


# Run the executable and save its stdout to log.csv.
def run_firmware_and_capture_log():
    if not EXE_PATH.exists():
        raise FileNotFoundError(f"{EXE_PATH} not found; build first.")

    print("Running firmware demo to generate log.csv ...")
    with LOG_FILE.open("w", encoding="utf-8") as f:
        # stdout of the program will be written directly into log.csv
        result = subprocess.run(
            [str(EXE_PATH)],
            stdout=f,
            stderr=subprocess.PIPE,
            text=True,
        )

    if result.returncode != 0:
        print("❌ Firmware run failed:")
        print(result.stderr)
        raise SystemExit(result.returncode)

    print("✅ Log written to", LOG_FILE)


# Load log.csv into numpy arrays.
def load_log():
    print("Loading", LOG_FILE)
    data = np.loadtxt(LOG_FILE, delimiter=",")

    # Expecting: step_index, setpoint, speed, control_output
    step = data[:, 0]
    setpoint = data[:, 1]
    speed = data[:, 2]
    control = data[:, 3]

    return step, setpoint, speed, control


# Plot setpoint vs speed, and control output vs time.
def plot_response(step, setpoint, speed, control):
    # Time axis assuming dt = 0.01s in main.c
    dt = 0.01
    t = step * dt

    # Setpoint vs speed
    plt.figure()
    plt.plot(t, setpoint, label="Setpoint")
    plt.plot(t, speed, label="Speed")
    plt.xlabel("Time (s)")
    plt.ylabel("Speed (arb. units)")
    plt.title("PID Step Response")
    plt.legend()
    plt.grid(True)

    # Control output
    plt.figure()
    plt.plot(t, control, label="Control output (u)")
    plt.xlabel("Time (s)")
    plt.ylabel("u")
    plt.title("Control Effort")
    plt.legend()
    plt.grid(True)

    plt.show()


def main():
    build_firmware()
    run_firmware_and_capture_log()
    step, setpoint, speed, control = load_log()
    plot_response(step, setpoint, speed, control)


if __name__ == "__main__":
    main()
