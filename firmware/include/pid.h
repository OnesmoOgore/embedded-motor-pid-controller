/**
 * @file    pid.h
 * @brief   Production-ready PID controller implementation for embedded systems
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
 * This file provides a complete PID (Proportional-Integral-Derivative) controller
 * implementation designed for embedded motor control applications. The implementation
 * includes advanced features:
 *
 * - **Anti-windup protection**: Prevents integrator saturation during output limiting
 * - **Derivative-on-measurement**: Eliminates derivative kick on setpoint changes
 * - **Derivative filtering**: Optional low-pass filter for noise reduction
 * - **Reentrant design**: Multiple independent PID instances supported
 * - **Platform-agnostic**: Pure C99, no external dependencies
 *
 * @section USAGE
 *
 * Basic usage example:
 * @code
 * #include "pid.h"
 *
 * pid_t motor_controller;
 *
 * // Initialize with gains: Kp=2.0, Ki=0.5, Kd=0.1, sample time=10ms
 * pid_init(&motor_controller, 2.0f, 0.5f, 0.1f, 0.01f, -100.0f, 100.0f);
 *
 * // In periodic control loop (every 10ms)
 * float setpoint = 100.0f;  // Target RPM
 * float measurement = read_motor_speed();
 * float control_output = pid_compute(&motor_controller, setpoint, measurement);
 * set_motor_pwm(control_output);
 * @endcode
 *
 * Advanced usage with filtering:
 * @code
 * pid_init_advanced(&motor_controller,
 *                   2.0f, 0.5f, 0.1f, 0.01f,  // Gains and sample time
 *                   -100.0f, 100.0f,          // Output limits
 *                   -50.0f, 50.0f,            // Integrator limits
 *                   0.8f);                    // Derivative filter (0-1)
 * @endcode
 *
 * @section ALGORITHM
 *
 * Control equation:
 *   output = Kp*error + Ki*∫error*dt + Kd*d(measurement)/dt
 *
 * Where:
 * - error = setpoint - measurement
 * - Derivative calculated on measurement to avoid derivative kick
 * - Integrator clamped to prevent windup
 * - Output clamped to specified limits
 *
 * @section THREAD_SAFETY
 *
 * This implementation is reentrant but NOT thread-safe. If accessing the same
 * PID instance from multiple threads, external synchronization is required.
 * Multiple independent instances can be used safely in parallel.
 */

#ifndef PID_H_
#define PID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <stdint.h>    /**< Standard integer types */

/*============================================================================*/
/* DATA TYPES AND STRUCTURES                                                 */
/*============================================================================*/

/**
 * @brief PID Controller instance structure
 *
 * This structure contains all configuration parameters and internal state
 * for a single PID control loop. The structure is designed to be:
 * - Reentrant: Multiple instances can coexist
 * - Persistent: Maintains state between calls to pid_compute()
 * - Self-contained: No global variables
 *
 * @note All members should be treated as internal implementation details.
 *       Use the provided API functions for initialization and operation.
 *
 * @warning Do not modify structure members directly after initialization.
 *          Use pid_reset() to clear state or re-initialize for new gains.
 */
typedef struct {
    /*------------------------------------------------------------------------*/
    /* Configuration Parameters (set during initialization)                  */
    /*------------------------------------------------------------------------*/

    float kp;              /**< Proportional gain
                            *   Controls immediate response to error.
                            *   Higher values = faster response but may overshoot.
                            *   Typical range: 0.1 to 10.0 */

    float ki;              /**< Integral gain
                            *   Eliminates steady-state error over time.
                            *   Higher values = faster error elimination but may oscillate.
                            *   Typical range: 0.0 to 5.0
                            *   Set to 0.0 to disable integral action */

    float kd;              /**< Derivative gain
                            *   Dampens oscillations and improves stability.
                            *   Higher values = more damping but sensitive to noise.
                            *   Typical range: 0.0 to 1.0
                            *   Set to 0.0 to disable derivative action */

    float dt;              /**< Sample time in seconds
                            *   Time between successive calls to pid_compute().
                            *   Must match actual control loop period.
                            *   Typical range: 0.001 to 1.0 seconds
                            *   Example: 0.01 = 10ms = 100Hz update rate */

    float out_min;         /**< Minimum output limit
                            *   Lower bound for control output.
                            *   Prevents actuator over-driving in negative direction.
                            *   Example: -100.0 for -100% PWM */

    float out_max;         /**< Maximum output limit
                            *   Upper bound for control output.
                            *   Prevents actuator over-driving in positive direction.
                            *   Example: 100.0 for +100% PWM */

    float integrator_min;  /**< Minimum integrator limit (anti-windup)
                            *   Lower bound for integral accumulator.
                            *   Prevents excessive negative integrator buildup.
                            *   Automatically set if using pid_init() */

    float integrator_max;  /**< Maximum integrator limit (anti-windup)
                            *   Upper bound for integral accumulator.
                            *   Prevents excessive positive integrator buildup.
                            *   Automatically set if using pid_init() */

    float derivative_lpf;  /**< Derivative low-pass filter coefficient
                            *   Range: 0.0 (no filtering) to 1.0 (maximum filtering).
                            *   Higher values = more filtering = less noise sensitivity.
                            *   Recommended: 0.7 to 0.9 for noisy measurements.
                            *   Formula: filtered = α*filtered + (1-α)*raw
                            *   Set to 0.0 to disable filtering (default) */

    /*------------------------------------------------------------------------*/
    /* Internal State Variables (modified during operation)                  */
    /*------------------------------------------------------------------------*/

    float integrator;          /**< Integral accumulator
                                *   Sum of errors over time (∫error*dt).
                                *   Automatically clamped to integrator_min/max.
                                *   Reset to 0 by pid_reset() */

    float prev_error;          /**< Previous error value
                                *   Stored for future use (currently unused but kept
                                *   for backward compatibility and potential extensions).
                                *   Updated each call to pid_compute() */

    float prev_measurement;    /**< Previous measurement value
                                *   Used to calculate derivative-on-measurement:
                                *   derivative = -(measurement - prev_measurement)/dt
                                *   Negative sign maintains correct control direction.
                                *   Reset to 0 by pid_reset() */

    float derivative_filtered; /**< Filtered derivative value
                                *   Exponentially filtered derivative to reduce noise.
                                *   Only used if derivative_lpf > 0.
                                *   Reset to 0 by pid_reset() */
} pid_t;

/*============================================================================*/
/* PUBLIC API FUNCTIONS                                                      */
/*============================================================================*/

/**
 * @brief Initialize a PID controller with standard configuration
 *
 * This function initializes a PID controller instance with the specified gains
 * and limits. Integrator limits are automatically calculated based on output
 * limits and Ki gain. No derivative filtering is applied (derivative_lpf = 0).
 *
 * This is the recommended initialization method for most applications. For
 * fine-grained control over integrator limits and derivative filtering, use
 * pid_init_advanced() instead.
 *
 * @param[out] pid         Pointer to PID structure to initialize
 *                         Must not be NULL
 * @param[in]  kp          Proportional gain (typically 0.1 to 10.0)
 *                         Higher = faster response, more overshoot
 * @param[in]  ki          Integral gain (typically 0.0 to 5.0)
 *                         Set to 0.0 to disable integral action
 *                         Higher = faster steady-state error elimination
 * @param[in]  kd          Derivative gain (typically 0.0 to 1.0)
 *                         Set to 0.0 to disable derivative action
 *                         Higher = more damping, noise sensitive
 * @param[in]  dt          Sample time in seconds (must match loop period)
 *                         Example: 0.01 for 10ms (100Hz) control loop
 *                         Must be > 0
 * @param[in]  out_min     Minimum output limit (e.g., -100.0 for -100% PWM)
 *                         Must be < out_max
 * @param[in]  out_max     Maximum output limit (e.g., 100.0 for +100% PWM)
 *                         Must be > out_min
 *
 * @return None
 *
 * @note All internal state variables are initialized to zero
 * @note Integrator limits automatically set to: out_min/Ki and out_max/Ki
 * @note No derivative filtering is applied (can be added with pid_init_advanced)
 *
 * @warning Ensure dt matches your actual control loop period for correct behavior
 * @warning This function does NOT validate input parameters for performance
 *
 * @par Example:
 * @code
 * pid_t temperature_controller;
 *
 * // Initialize for 75°C temperature control, 100ms update rate
 * pid_init(&temperature_controller,
 *          2.0f,    // Kp
 *          0.5f,    // Ki
 *          0.1f,    // Kd
 *          0.1f,    // dt = 100ms
 *          0.0f,    // Min PWM = 0%
 *          100.0f); // Max PWM = 100%
 * @endcode
 *
 * @see pid_init_advanced() for more control over anti-windup and filtering
 * @see pid_compute() to calculate control output
 * @see pid_reset() to clear internal state
 */
void pid_init(pid_t *pid,
              float kp,
              float ki,
              float kd,
              float dt,
              float out_min,
              float out_max);

/**
 * @brief Initialize a PID controller with advanced configuration options
 *
 * This function provides fine-grained control over all PID parameters including
 * custom integrator limits for anti-windup and derivative low-pass filtering.
 * Use this when the automatic integrator limits from pid_init() are not suitable
 * or when derivative filtering is needed for noisy measurements.
 *
 * @param[out] pid             Pointer to PID structure to initialize
 *                             Must not be NULL
 * @param[in]  kp              Proportional gain
 * @param[in]  ki              Integral gain (0.0 to disable)
 * @param[in]  kd              Derivative gain (0.0 to disable)
 * @param[in]  dt              Sample time in seconds (must match loop period)
 * @param[in]  out_min         Minimum output limit
 * @param[in]  out_max         Maximum output limit
 * @param[in]  integrator_min  Minimum integrator limit for anti-windup
 *                             Prevents excessive negative buildup
 *                             Typical: out_min / ki
 * @param[in]  integrator_max  Maximum integrator limit for anti-windup
 *                             Prevents excessive positive buildup
 *                             Typical: out_max / ki
 * @param[in]  derivative_lpf  Derivative low-pass filter coefficient (0.0 to 1.0)
 *                             0.0 = no filtering (raw derivative)
 *                             1.0 = maximum filtering (slowest response)
 *                             Recommended: 0.7 to 0.9 for noisy sensors
 *
 * @return None
 *
 * @note The derivative_lpf parameter will be clamped to [0.0, 1.0] range
 * @note All internal state variables are initialized to zero
 *
 * @par Example (with derivative filtering for noisy encoder):
 * @code
 * pid_t motor_controller;
 *
 * pid_init_advanced(&motor_controller,
 *                   1.5f,      // Kp
 *                   0.3f,      // Ki
 *                   0.2f,      // Kd
 *                   0.01f,     // dt = 10ms
 *                   -100.0f,   // Min output
 *                   100.0f,    // Max output
 *                   -200.0f,   // Min integrator (custom)
 *                   200.0f,    // Max integrator (custom)
 *                   0.85f);    // 85% derivative filtering
 * @endcode
 *
 * @see pid_init() for simpler initialization with automatic settings
 * @see pid_compute() to calculate control output
 */
void pid_init_advanced(pid_t *pid,
                      float kp,
                      float ki,
                      float kd,
                      float dt,
                      float out_min,
                      float out_max,
                      float integrator_min,
                      float integrator_max,
                      float derivative_lpf);

/**
 * @brief Calculate PID control output
 *
 * Computes the control output based on the error between setpoint and
 * measurement using the PID algorithm with anti-windup and optional
 * derivative filtering.
 *
 * This function should be called periodically at the rate specified by
 * the dt parameter during initialization. Irregular calling intervals
 * will result in incorrect integral and derivative calculations.
 *
 * Algorithm:
 * 1. Calculate error = setpoint - measurement
 * 2. Proportional term: P = Kp * error
 * 3. Integral term: I = Ki * ∫error*dt (with anti-windup clamping)
 * 4. Derivative term: D = -Kd * d(measurement)/dt (on measurement, not error)
 * 5. Output = P + I + D (clamped to out_min/out_max)
 *
 * @param[in,out] pid         Pointer to initialized PID structure
 *                            Internal state is updated each call
 *                            Must not be NULL
 * @param[in]     setpoint    Target value (desired process variable)
 *                            Examples: target RPM, temperature, position
 * @param[in]     measurement Current process variable (sensor reading)
 *                            Same units as setpoint
 *
 * @return Control output clamped to [out_min, out_max] range
 *         Units determined by application (e.g., PWM duty cycle, voltage)
 *
 * @note Uses derivative-on-measurement to avoid "derivative kick" when
 *       setpoint changes suddenly
 * @note Integrator is automatically clamped to prevent windup
 * @note This function modifies internal state (integrator, prev_measurement, etc.)
 *
 * @warning Must be called at regular intervals matching the dt parameter
 * @warning Ensure pid has been initialized before calling this function
 *
 * @par Example (periodic timer interrupt):
 * @code
 * // Called every 10ms by timer interrupt
 * void TIM_IRQHandler(void) {
 *     float current_speed = read_encoder_rpm();
 *     float target_speed = 1000.0f;  // 1000 RPM
 *
 *     float pwm_duty = pid_compute(&motor_pid, target_speed, current_speed);
 *     set_motor_pwm(pwm_duty);
 * }
 * @endcode
 *
 * @see pid_init() to initialize before first use
 * @see pid_reset() to clear state (e.g., when switching setpoints)
 */
float pid_compute(pid_t *pid, float setpoint, float measurement);

/**
 * @brief Reset PID controller internal state
 *
 * Clears all internal state variables (integrator, previous error, previous
 * measurement, filtered derivative) while preserving the configuration
 * parameters (gains, limits, sample time).
 *
 * Use this function when:
 * - Starting control after a period of inactivity
 * - Changing setpoint by a large amount
 * - Recovering from a fault condition
 * - Switching between different operating modes
 *
 * @param[in,out] pid Pointer to PID structure to reset
 *                    Configuration parameters remain unchanged
 *                    Must not be NULL
 *
 * @return None
 *
 * @note Does NOT reset gains, limits, or sample time
 * @note Safe to call at any time, even during active control
 * @note Equivalent to re-initializing with same parameters
 *
 * @par Example (mode switch):
 * @code
 * // Switch from position to velocity control
 * pid_reset(&position_controller);  // Clear old state
 * float new_setpoint = 500.0f;      // New target
 * // Resume control with fresh start
 * @endcode
 *
 * @see pid_init() to reset both state and configuration
 */
void pid_reset(pid_t *pid);

/*============================================================================*/
/* END OF PUBLIC API                                                         */
/*============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* PID_H_ */
