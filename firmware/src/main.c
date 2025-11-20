/**
 * @file    main.c
 * @brief   Demo application for PID motor controller
 * @author  Onesmo Ogore
 * @date    November 17, 2025
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Onesmo Ogore
 * @license MIT License
 *
 * SPDX-License-Identifier: MIT
 *
 * @section DESCRIPTION
 *
 * This file demonstrates a complete PID-based motor control application.
 * It serves multiple purposes:
 *
 * 1. **Desktop Simulation**: Runs on desktop (Linux, macOS, Windows) for
 *    algorithm testing and PID gain tuning before hardware deployment
 *
 * 2. **Example Integration**: Shows how to integrate the PID controller
 *    library into a complete application
 *
 * 3. **Template for Embedded**: Provides a starting point for embedded
 *    implementation - replace simulation with hardware-specific code
 *
 * @section PROGRAM_FLOW
 *
 * The program flow is:
 *
 * 1. **Initialization Phase**:
 *    - Initialize motor hardware/simulation (motor_init)
 *    - Configure PID controller with tuned gains (pid_init)
 *    - Set control loop parameters (setpoint, sample time)
 *
 * 2. **Control Loop Phase** (repeated every sample time):
 *    - Read motor speed from sensor/encoder (motor_get_speed)
 *    - Calculate control output via PID (pid_compute)
 *    - Apply control output to motor (motor_set_output)
 *    - Log data for analysis (optional, desktop only)
 *
 * 3. **Shutdown Phase** (not implemented in this demo):
 *    - Stop motor safely
 *    - Cleanup resources
 *
 * @section SIMULATION_MODE
 *
 * This demo runs in **simulation mode** for desktop testing:
 * - Uses simple motor model instead of real hardware
 * - Runs in a simple for-loop instead of timer interrupt
 * - Outputs CSV data to stdout for plotting
 * - No actual GPIO, PWM, or timers used
 *
 * Output format (CSV):
 *   step,setpoint,measurement,output
 *   0,3.0000,0.0000,0.0000
 *   1,3.0000,0.0096,2.2400
 *   ...
 *
 * This output is processed by sim/pid_simulation.py for visualization.
 *
 * @section EMBEDDED_MODE
 *
 * To convert this demo for real embedded hardware:
 *
 * 1. **Replace motor_init()**:
 *    - Initialize PWM timer for motor control
 *    - Configure GPIO for direction control
 *    - Initialize encoder timer for speed measurement
 *    - Enable motor driver
 *
 * 2. **Replace motor_get_speed()**:
 *    - Read encoder counter
 *    - Calculate actual motor RPM
 *    - Apply calibration/scaling
 *
 * 3. **Replace motor_set_output()**:
 *    - Set PWM duty cycle
 *    - Control direction pins
 *
 * 4. **Replace control loop**:
 *    - Use timer interrupt (e.g., 10ms periodic)
 *    - Or RTOS task with precise timing
 *    - Remove printf (or use UART for debugging)
 *
 * Example timer interrupt structure:
 * @code
 * void TIM2_IRQHandler(void) {
 *     static float setpoint = 1000.0f;  // Target RPM
 *
 *     float speed = motor_get_speed();
 *     float output = pid_compute(&motor_pid, setpoint, speed);
 *     motor_set_output(output);
 *
 *     TIM2->SR &= ~TIM_SR_UIF;  // Clear interrupt flag
 * }
 * @endcode
 *
 * @section PID_TUNING
 *
 * The PID gains in this demo are tuned for the simulation model:
 * - Kp = 0.8  (Proportional - immediate response to error)
 * - Ki = 0.3  (Integral - eliminates steady-state error)
 * - Kd = 0.05 (Derivative - dampens oscillations)
 *
 * For real hardware, you will need to re-tune these gains using methods like:
 * - Manual tuning (trial and error)
 * - Ziegler-Nichols method
 * - Relay auto-tuning
 * - Model-based tuning
 *
 * Tuning guidelines:
 * 1. Start with Kp only (Ki=0, Kd=0)
 * 2. Increase Kp until system oscillates
 * 3. Add Ki to eliminate steady-state error
 * 4. Add Kd to reduce overshoot and oscillation
 * 5. Fine-tune all three for optimal performance
 *
 * @section SAFETY_CONSIDERATIONS
 *
 * When adapting this code for real hardware:
 * - Implement output rate limiting (prevent sudden changes)
 * - Add fault detection (overcurrent, overspeed, encoder loss)
 * - Implement emergency stop functionality
 * - Add watchdog timer for system monitoring
 * - Validate sensor readings for sanity
 * - Implement soft start/stop ramps
 *
 * @note This demo is intentionally simple for clarity. Production code
 *       should include error handling, safety features, and robustness.
 */

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "motor.h"  /**< Motor hardware abstraction layer */
#include "pid.h"    /**< PID controller library */
#include <stdio.h>  /**< Standard I/O for simulation output */

/*============================================================================*/
/* APPLICATION CONFIGURATION                                                 */
/*============================================================================*/

/**
 * @brief Number of control loop iterations to run
 *
 * For simulation: Determines how many time steps to simulate
 * For embedded: This would be infinite (while(1) loop) or RTOS-managed
 *
 * At 10ms sample time:
 * - 1000 steps = 10 seconds of simulation
 * - 6000 steps = 60 seconds = 1 minute
 */
#define NUM_CONTROL_ITERATIONS  1000

/**
 * @brief Target motor speed (setpoint)
 *
 * Units: Arbitrary for simulation (matches motor model units)
 * For embedded: RPM, rad/s, or application-specific units
 *
 * This is the desired speed the PID controller will try to maintain.
 */
#define TARGET_SPEED  3.0f

/*============================================================================*/
/* PID CONTROLLER CONFIGURATION                                              */
/*============================================================================*/

/**
 * @brief Proportional gain (Kp)
 *
 * Controls immediate response to error:
 * - Higher Kp = faster response, but more overshoot
 * - Lower Kp = slower response, but more stable
 *
 * Tuned for simulation model. Adjust for real hardware.
 */
#define PID_KP  0.8f

/**
 * @brief Integral gain (Ki)
 *
 * Eliminates steady-state error over time:
 * - Higher Ki = faster error elimination, but may cause oscillation
 * - Lower Ki = slower error elimination, but more stable
 * - Ki = 0 disables integral action (PI → P controller)
 *
 * Tuned for simulation model. Adjust for real hardware.
 */
#define PID_KI  0.3f

/**
 * @brief Derivative gain (Kd)
 *
 * Dampens oscillations and improves stability:
 * - Higher Kd = more damping, but noise-sensitive
 * - Lower Kd = less damping, less noise sensitivity
 * - Kd = 0 disables derivative action (PID → PI controller)
 *
 * Tuned for simulation model. Adjust for real hardware.
 */
#define PID_KD  0.05f

/**
 * @brief Sample time / control loop period (seconds)
 *
 * Time between successive PID calculations and motor updates.
 * Must match the actual control loop execution rate.
 *
 * Common values:
 * - 0.001 (1ms)   - High-performance servo control
 * - 0.01  (10ms)  - Standard motor control (used here)
 * - 0.1   (100ms) - Slow processes (temperature, etc.)
 *
 * @warning This must match your actual loop timing for correct PID behavior!
 */
#define PID_SAMPLE_TIME  0.01f  /* 10ms = 100Hz control rate */

/**
 * @brief Minimum output limit (duty cycle)
 *
 * Prevents excessive negative (reverse) motor drive.
 * Range: -1.0 = full reverse
 *
 * For unidirectional control, set to 0.0
 */
#define OUTPUT_MIN  -1.0f

/**
 * @brief Maximum output limit (duty cycle)
 *
 * Prevents excessive positive (forward) motor drive.
 * Range: +1.0 = full forward
 */
#define OUTPUT_MAX  1.0f

/*============================================================================*/
/* MAIN APPLICATION                                                          */
/*============================================================================*/

/**
 * @brief Main application entry point
 *
 * Implements a simple PID motor control demonstration:
 * 1. Initializes motor and PID controller
 * 2. Runs control loop for specified number of iterations
 * 3. Outputs data in CSV format for analysis
 *
 * For embedded deployment:
 * - Replace printf with UART logging or remove
 * - Replace for-loop with timer interrupt or RTOS task
 * - Add error handling and safety features
 * - Implement user interface (buttons, display, etc.)
 *
 * @return 0 on success (simulation mode)
 *         Never returns in embedded mode (infinite loop)
 */
int main(void)
{
    /*------------------------------------------------------------------------*/
    /* Initialization Phase                                                  */
    /*------------------------------------------------------------------------*/

    /* Initialize motor hardware (simulation or real hardware)
     *
     * For simulation: Resets motor model to initial state
     * For embedded: Configures PWM, GPIO, encoder, etc.
     */
    motor_init();

    /* Create and initialize PID controller instance
     *
     * This configures the PID algorithm with tuned gains and limits.
     * The controller starts with zero internal state (no integral accumulation,
     * no derivative history).
     */
    pid_t pid;
    pid_init(&pid,
             PID_KP,          /* Proportional gain */
             PID_KI,          /* Integral gain */
             PID_KD,          /* Derivative gain */
             PID_SAMPLE_TIME, /* Sample time (must match loop rate!) */
             OUTPUT_MIN,      /* Minimum output (full reverse) */
             OUTPUT_MAX);     /* Maximum output (full forward) */

    /* Set target speed (setpoint)
     *
     * This is the desired speed the controller will try to achieve and maintain.
     * For embedded: This could come from user input, network, or sensor.
     */
    float setpoint = TARGET_SPEED;

    /*------------------------------------------------------------------------*/
    /* Control Loop Phase                                                    */
    /*------------------------------------------------------------------------*/

    /* Main control loop - runs periodically at sample rate
     *
     * SIMULATION MODE (current implementation):
     * - Simple for-loop with fixed iteration count
     * - Outputs CSV data to stdout for plotting
     * - Terminates after NUM_CONTROL_ITERATIONS steps
     *
     * EMBEDDED MODE (typical implementation):
     * - Timer interrupt every PID_SAMPLE_TIME seconds
     * - Or RTOS task with precise period
     * - Runs forever (infinite loop or RTOS scheduler)
     * - No printf (use UART, display, or data logging)
     */
    for (int step = 0; step < NUM_CONTROL_ITERATIONS; ++step)
    {
        /* 1. READ: Get current motor speed from sensor/encoder
         *
         * Simulation: Returns simulated motor speed (perfect measurement)
         * Embedded: Reads encoder, calculates RPM, applies filtering
         */
        float speed = motor_get_speed();

        /* 2. COMPUTE: Calculate PID control output
         *
         * The PID algorithm computes the control effort needed to
         * minimize the error between setpoint and measured speed.
         *
         * Returns a value in range [OUTPUT_MIN, OUTPUT_MAX]
         */
        float u = pid_compute(&pid, setpoint, speed);

        /* 3. ACTUATE: Apply control output to motor
         *
         * Simulation: Updates motor model dynamics
         * Embedded: Sets PWM duty cycle and direction
         */
        motor_set_output(u);

        /* 4. LOG: Output data for analysis (simulation only)
         *
         * Format: step,setpoint,measurement,output
         * This CSV data is captured by sim/pid_simulation.py for plotting
         *
         * For embedded: Remove or replace with UART logging
         */
        printf("%d,%.4f,%.4f,%.4f\n", step, setpoint, speed, u);

        /* NOTE: In this simulation, there's no explicit delay because
         * the motor model is updated synchronously. In real hardware,
         * this loop would be driven by a timer interrupt at the correct
         * sample rate (e.g., 10ms = 100Hz).
         */
    }

    /*------------------------------------------------------------------------*/
    /* Shutdown Phase (simulation only)                                     */
    /*------------------------------------------------------------------------*/

    /* For simulation: Program exits normally
     * For embedded: Typically never reaches here (infinite loop above)
     *
     * If shutdown is needed (embedded):
     * - Stop motor safely (ramp down, then disable)
     * - Save configuration to non-volatile memory
     * - Enter low-power mode
     */

    return 0;  /* Success (simulation mode only) */

    /*------------------------------------------------------------------------*/
    /* Example Embedded Implementation (Timer Interrupt)                    */
    /*------------------------------------------------------------------------*/
    /*
     * // Global PID instance (accessible from interrupt)
     * pid_t g_motor_pid;
     * float g_setpoint = 1000.0f;  // Target RPM
     *
     * void system_init(void) {
     *     motor_init();
     *     pid_init(&g_motor_pid, 1.0f, 0.5f, 0.1f, 0.01f, -100.0f, 100.0f);
     *
     *     // Configure timer for 10ms periodic interrupt
     *     TIM2_Init(100);  // 100Hz = 10ms period
     *     TIM2_EnableInterrupt();
     * }
     *
     * // Timer interrupt - called every 10ms
     * void TIM2_IRQHandler(void) {
     *     float speed = motor_get_speed();
     *     float output = pid_compute(&g_motor_pid, g_setpoint, speed);
     *     motor_set_output(output);
     *
     *     TIM2_ClearInterruptFlag();
     * }
     *
     * int main(void) {
     *     system_init();
     *     while (1) {
     *         // Main loop handles non-time-critical tasks:
     *         // - User interface
     *         // - Communication
     *         // - Data logging
     *         // - Fault monitoring
     *     }
     * }
     */
}

/*============================================================================*/
/* END OF FILE                                                               */
/*============================================================================*/
