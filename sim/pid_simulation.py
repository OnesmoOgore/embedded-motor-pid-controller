#!/usr/bin/env python3
"""
PID Controller Simulation and Validation Tool

This script provides desktop simulation and visualization of the PID motor
controller before embedded hardware deployment. It compiles the firmware
code, executes it in simulation mode, captures the control loop data, and
generates response plots for analysis.

Author:  Onesmo Ogore
Date:    November 17, 2025
Version: 1.0.0
License: MIT

Usage:
    python sim/pid_simulation.py

Output:
    - sim/log.csv: Raw simulation data (step, setpoint, measurement, output)
    - step_response.png: Visualization of PID step response

Features:
    - Automatic firmware compilation with GCC
    - Cross-platform support (Windows, Linux, macOS)
    - CI/CD integration (headless plotting)
    - Step response analysis
    - Control effort visualization

Requirements:
    - GCC compiler (for firmware compilation)
    - Python 3.11+
    - numpy>=1.24.0
    - matplotlib>=3.7.0

SPDX-License-Identifier: MIT
"""

import subprocess
import sys
from pathlib import Path
from typing import Tuple

import numpy as np
import os
import matplotlib

# Use non-GUI backend when running in CI environment
if os.environ.get("CI") == "true":
    matplotlib.use("Agg")

import matplotlib.pyplot as plt

#===============================================================================
# CONFIGURATION
#===============================================================================

# Project directory structure
ROOT = Path(__file__).resolve().parents[1]   # Repository root
FIRMWARE_SRC = ROOT / "firmware" / "src"     # C source files
FIRMWARE_INC = ROOT / "firmware" / "include" # C header files
BUILD_DIR = ROOT / "build"                   # Build artifacts
LOG_FILE = ROOT / "sim" / "log.csv"          # Simulation output

# Platform-specific executable name
EXE_NAME = "pid_demo.exe" if sys.platform.startswith("win") else "pid_demo"
EXE_PATH = BUILD_DIR / EXE_NAME

# Simulation parameters (must match main.c configuration)
SAMPLE_TIME_SEC = 0.01  # 10ms control loop period (100Hz)

#===============================================================================
# BUILD FUNCTIONS
#===============================================================================

def build_firmware() -> None:
    """
    Compile firmware sources into desktop executable.

    Compiles the PID controller firmware (main.c, pid.c, motor.c) into a
    standalone executable for desktop simulation. Uses GCC with strict
    warnings enabled for code quality validation.

    Compiler flags:
        -Wall:   Enable all common warnings
        -Wextra: Enable extra warnings beyond -Wall
        -Werror: Treat warnings as errors (ensures clean code)
        -I:      Include directory for headers

    Raises:
        SystemExit: If compilation fails (non-zero return code)

    Side effects:
        - Creates build/ directory if it doesn't exist
        - Generates executable at BUILD_DIR/EXE_NAME
        - Prints build commands and status to stdout
    """
    BUILD_DIR.mkdir(exist_ok=True)

    # GCC compilation command
    cmd = [
        "gcc",
        "-Wall",                          # Enable all warnings
        "-Wextra",                        # Enable extra warnings
        "-Werror",                        # Treat warnings as errors
        f"-I{FIRMWARE_INC}",              # Include path for headers
        str(FIRMWARE_SRC / "main.c"),     # Main application
        str(FIRMWARE_SRC / "pid.c"),      # PID controller implementation
        str(FIRMWARE_SRC / "motor.c"),    # Motor simulation model
        "-o",
        str(EXE_PATH),                    # Output executable path
    ]

    print("=" * 70)
    print("Building firmware for desktop simulation...")
    print("=" * 70)
    print("Command:", " ".join(cmd))
    print()

    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
        print("[FAIL] BUILD FAILED")
        print("\nCompiler output:")
        print(result.stdout)
        print("\nCompiler errors:")
        print(result.stderr)
        raise SystemExit(result.returncode)

    print(f"[OK] Build succeeded: {EXE_PATH}")
    print()

#===============================================================================
# SIMULATION FUNCTIONS
#===============================================================================

def run_firmware_and_capture_log() -> None:
    """
    Execute firmware simulation and capture control loop data.

    Runs the compiled firmware executable, which simulates the PID motor
    control loop. The program outputs CSV-formatted data (step, setpoint,
    measurement, output) to stdout, which is redirected to log.csv.

    The simulation runs for NUM_CONTROL_ITERATIONS steps (defined in main.c),
    typically 1000 steps = 10 seconds at 10ms sample time.

    Output format (CSV with header):
        step,setpoint,measurement,output
        0, 3.0000, 0.0000, 0.0000
        1, 3.0000, 0.0096, 2.2400
        ...

    Raises:
        FileNotFoundError: If executable doesn't exist (build first)
        SystemExit: If simulation fails (non-zero return code)

    Side effects:
        - Creates/overwrites sim/log.csv with simulation data
        - Prints execution status to stdout
    """
    if not EXE_PATH.exists():
        raise FileNotFoundError(
            f"Executable not found: {EXE_PATH}\n"
            f"Run build_firmware() first."
        )

    print("=" * 70)
    print("Running firmware simulation...")
    print("=" * 70)
    print(f"Executable: {EXE_PATH}")
    print(f"Output:     {LOG_FILE}")
    print()

    with LOG_FILE.open("w", encoding="utf-8") as f:
        # Redirect stdout of firmware executable directly to log.csv
        result = subprocess.run(
            [str(EXE_PATH)],
            stdout=f,                    # Capture stdout to file
            stderr=subprocess.PIPE,      # Capture stderr for error handling
            text=True,
        )

    if result.returncode != 0:
        print("[FAIL] SIMULATION FAILED")
        print("\nError output:")
        print(result.stderr)
        raise SystemExit(result.returncode)

    print(f"[OK] Simulation complete: {LOG_FILE}")
    print()

#===============================================================================
# DATA ANALYSIS FUNCTIONS
#===============================================================================

def load_log() -> Tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """
    Load and parse simulation data from CSV log file.

    Reads the CSV output from the firmware simulation and extracts the
    four data columns into numpy arrays for analysis and plotting.

    CSV format (with header):
        Header: step,setpoint,measurement,output
        Column 0: Step index (iteration counter)
        Column 1: Setpoint (target speed)
        Column 2: Measurement (actual motor speed)
        Column 3: Control output (PID output, duty cycle)

    Returns:
        tuple: (step, setpoint, speed, control) as numpy arrays

    Raises:
        FileNotFoundError: If log.csv doesn't exist
        ValueError: If CSV format is invalid or empty

    Example:
        >>> step, setpoint, speed, control = load_log()
        >>> print(f"Simulation ran for {len(step)} steps")
        Simulation ran for 1000 steps
    """
    print("=" * 70)
    print("Loading simulation data...")
    print("=" * 70)
    print(f"File: {LOG_FILE}")

    try:
        # Load CSV data (delimiter=comma, skip header row)
        data = np.loadtxt(LOG_FILE, delimiter=",", skiprows=1)
    except FileNotFoundError:
        raise FileNotFoundError(
            f"Log file not found: {LOG_FILE}\n"
            f"Run run_firmware_and_capture_log() first."
        )
    except ValueError as e:
        raise ValueError(
            f"Invalid CSV format in {LOG_FILE}\n"
            f"Expected: step,setpoint,measurement,output\n"
            f"Error: {e}"
        )

    # Validate data shape
    if data.ndim != 2 or data.shape[1] != 4:
        raise ValueError(
            f"Invalid data shape: {data.shape}\n"
            f"Expected: (num_steps, 4) columns"
        )

    # Extract columns
    step = data[:, 0]       # Step index
    setpoint = data[:, 1]   # Target speed (setpoint)
    speed = data[:, 2]      # Measured speed (process variable)
    control = data[:, 3]    # Control output (manipulated variable)

    print(f"[OK] Loaded {len(step)} data points")
    print(f"     Time span: {len(step) * SAMPLE_TIME_SEC:.2f} seconds")
    print(f"     Setpoint range: [{setpoint.min():.3f}, {setpoint.max():.3f}]")
    print(f"     Speed range: [{speed.min():.3f}, {speed.max():.3f}]")
    print(f"     Control range: [{control.min():.3f}, {control.max():.3f}]")
    print()

    return step, setpoint, speed, control

#===============================================================================
# VISUALIZATION FUNCTIONS
#===============================================================================

def plot_response(step: np.ndarray,
                  setpoint: np.ndarray,
                  speed: np.ndarray,
                  control: np.ndarray) -> None:
    """
    Generate and save PID step response plots.

    Creates two publication-quality plots for PID controller performance
    analysis:

    1. **Step Response Plot**: Shows setpoint vs measured speed over time
       - Useful for analyzing: rise time, overshoot, settling time, steady-state error
       - Key metrics can be visually assessed from this plot

    2. **Control Effort Plot**: Shows PID output (duty cycle) over time
       - Useful for analyzing: control saturation, aggressiveness, stability
       - High-frequency oscillations indicate derivative noise or poor tuning

    Args:
        step: Step index array (iteration counter)
        setpoint: Target speed array
        speed: Measured speed array (process variable)
        control: Control output array (manipulated variable)

    Side effects:
        - Creates step_response.png in current directory
        - Shows interactive plot window (unless in CI mode)
        - Prints save confirmation to stdout

    Performance analysis:
        - Rise time: Time to reach 90% of final value
        - Overshoot: Peak value exceeding setpoint
        - Settling time: Time to stay within 5% of setpoint
        - Steady-state error: Final difference between setpoint and speed
    """
    print("=" * 70)
    print("Generating plots...")
    print("=" * 70)

    # Convert step index to time (seconds)
    # Assumes SAMPLE_TIME_SEC matches the dt in main.c
    time = step * SAMPLE_TIME_SEC

    # Create figure with two subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))

    #---------------------------------------------------------------------------
    # Plot 1: Step Response (Setpoint vs Measurement)
    #---------------------------------------------------------------------------
    ax1.plot(time, setpoint, 'r--', label='Setpoint', linewidth=2, alpha=0.8)
    ax1.plot(time, speed, 'b-', label='Measured Speed', linewidth=1.5)
    ax1.set_xlabel('Time (seconds)', fontsize=11)
    ax1.set_ylabel('Speed (arbitrary units)', fontsize=11)
    ax1.set_title('PID Controller Step Response', fontsize=13, fontweight='bold')
    ax1.legend(loc='best', fontsize=10)
    ax1.grid(True, alpha=0.3)

    # Calculate and display performance metrics
    final_speed = speed[-1]
    final_error = setpoint[-1] - final_speed
    overshoot = ((speed.max() - setpoint[-1]) / setpoint[-1] * 100
                 if speed.max() > setpoint[-1] else 0)

    # Add metrics text box
    metrics_text = (
        f'Final Speed: {final_speed:.3f}\n'
        f'Steady-State Error: {final_error:.3f}\n'
        f'Overshoot: {overshoot:.1f}%'
    )
    ax1.text(0.98, 0.02, metrics_text,
             transform=ax1.transAxes,
             fontsize=9,
             verticalalignment='bottom',
             horizontalalignment='right',
             bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

    #---------------------------------------------------------------------------
    # Plot 2: Control Effort
    #---------------------------------------------------------------------------
    ax2.plot(time, control, 'g-', label='Control Output', linewidth=1.5)
    ax2.axhline(y=1.0, color='r', linestyle='--', linewidth=1, alpha=0.5, label='Upper Limit')
    ax2.axhline(y=-1.0, color='r', linestyle='--', linewidth=1, alpha=0.5, label='Lower Limit')
    ax2.set_xlabel('Time (seconds)', fontsize=11)
    ax2.set_ylabel('Control Output (duty cycle)', fontsize=11)
    ax2.set_title('PID Control Effort', fontsize=13, fontweight='bold')
    ax2.legend(loc='best', fontsize=10)
    ax2.grid(True, alpha=0.3)
    ax2.set_ylim([-1.2, 1.2])  # Slightly beyond limits for visibility

    # Calculate control statistics
    control_mean = control.mean()
    control_std = control.std()
    saturation_time = np.sum(np.abs(control) >= 0.99) * SAMPLE_TIME_SEC

    # Add control statistics text box
    control_text = (
        f'Mean Output: {control_mean:.3f}\n'
        f'Std Dev: {control_std:.3f}\n'
        f'Saturation Time: {saturation_time:.2f}s'
    )
    ax2.text(0.98, 0.98, control_text,
             transform=ax2.transAxes,
             fontsize=9,
             verticalalignment='top',
             horizontalalignment='right',
             bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.5))

    #---------------------------------------------------------------------------
    # Save and display
    #---------------------------------------------------------------------------
    plt.tight_layout()
    plt.savefig("step_response.png", dpi=150, bbox_inches='tight')
    print("[OK] Plot saved: step_response.png")

    # Show interactive plot (unless in CI mode)
    if os.environ.get("CI") != "true":
        print("     Opening interactive plot window...")
        plt.show()
    else:
        print("     CI mode detected - skipping interactive display")

    print()

#===============================================================================
# MAIN PROGRAM
#===============================================================================

def main() -> None:
    """
    Main simulation workflow.

    Executes the complete PID simulation pipeline:
    1. Compile firmware sources to desktop executable
    2. Run simulation and capture control loop data
    3. Load and parse simulation results
    4. Generate performance analysis plots

    This workflow validates the PID controller implementation before
    deploying to embedded hardware, allowing for:
    - Algorithm verification
    - Gain tuning
    - Performance analysis
    - Documentation generation

    Raises:
        SystemExit: If any step fails (build, simulation, or plotting)
    """
    print()
    print("=" * 70)
    print("PID CONTROLLER SIMULATION TOOL")
    print("=" * 70)
    print()

    try:
        # Step 1: Build firmware for desktop
        build_firmware()

        # Step 2: Run simulation
        run_firmware_and_capture_log()

        # Step 3: Load results
        step, setpoint, speed, control = load_log()

        # Step 4: Visualize performance
        plot_response(step, setpoint, speed, control)

        print("=" * 70)
        print("SIMULATION COMPLETE - SUCCESS")
        print("=" * 70)
        print()
        print("Output files:")
        print(f"  - {LOG_FILE}")
        print(f"  - step_response.png")
        print()

    except Exception as e:
        print()
        print("=" * 70)
        print("SIMULATION FAILED - ERROR")
        print("=" * 70)
        print(f"Error: {e}")
        print()
        raise SystemExit(1)


if __name__ == "__main__":
    main()
