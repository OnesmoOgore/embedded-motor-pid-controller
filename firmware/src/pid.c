/**
 * @file    pid.c
 * @brief   Implementation of production-ready PID controller algorithm
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
 * This file implements a production-grade PID (Proportional-Integral-Derivative)
 * controller with advanced features for embedded motor control applications.
 *
 * Key implementation features:
 * - **Anti-windup protection**: Integrator clamping prevents saturation
 * - **Derivative-on-measurement**: Eliminates derivative kick on setpoint changes
 * - **Optional low-pass filtering**: Reduces derivative noise sensitivity
 * - **Reentrant design**: State stored in instance structure (no globals)
 * - **Efficient implementation**: Optimized for real-time embedded systems
 *
 * @section ALGORITHM_DETAILS
 *
 * The PID control algorithm computes:
 *   output = P + I + D
 *
 * Where:
 * - P = Kp × error
 * - I = Ki × ∫(error × dt)     [clamped to integrator_min/max]
 * - D = -Kd × d(measurement)/dt [negative sign for derivative-on-measurement]
 *
 * Anti-windup mechanism:
 * - Integrator is clamped independently from output
 * - Prevents excessive accumulation during saturation
 * - Enables faster recovery when error reduces
 *
 * Derivative-on-measurement:
 * - Calculates derivative based on measurement changes, not error changes
 * - Prevents "derivative kick" when setpoint changes suddenly
 * - Formula: d = -(measurement[n] - measurement[n-1]) / dt
 * - Negative sign maintains correct control direction
 *
 * @section PERFORMANCE
 *
 * Computational complexity per pid_compute() call:
 * - Multiplications: 5-6 (depending on filtering)
 * - Additions: 4-5
 * - Comparisons: 6-8 (clamping operations)
 * - Divisions: 1 (derivative calculation)
 *
 * Typical execution time on ARM Cortex-M4 @ 168MHz: <1µs
 *
 * @note All functions are reentrant and can be used with multiple PID instances
 * @note No dynamic memory allocation - all state in pid_t structure
 * @note No global variables - fully thread-safe with external synchronization
 */

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "pid.h"
#include <assert.h>   /* For parameter validation in debug builds */
#include <stddef.h>   /* For NULL definition */

/*============================================================================*/
/* PRIVATE HELPER FUNCTIONS                                                  */
/*============================================================================*/

/**
 * @brief Clamp a value to a specified range
 *
 * Restricts a floating-point value to lie within [min, max] bounds.
 * This is a utility function used internally for output limiting and
 * anti-windup protection.
 *
 * @param[in] value  Value to clamp
 * @param[in] min    Minimum allowed value
 * @param[in] max    Maximum allowed value
 * @return Clamped value in range [min, max]
 *
 * @note This function is static (private to this file)
 * @note If min > max, behavior is undefined (not validated for performance)
 */
static float clamp(float value, float min, float max)
{
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

/*============================================================================*/
/* PUBLIC API IMPLEMENTATION                                                 */
/*============================================================================*/

/**
 * @brief Initialize PID controller with standard configuration
 *
 * See detailed documentation in pid.h
 *
 * Implementation notes:
 * - All state variables initialized to zero
 * - Integrator limits calculated automatically: integrator_min/max = out_min/max / Ki
 * - Division-by-zero protection: if Ki=0, integrator limits set to output limits
 * - No derivative filtering enabled (derivative_lpf = 0)
 * - Takes ~10-20 CPU cycles on typical embedded processors
 */
void pid_init(pid_t *pid,
              float kp,
              float ki,
              float kd,
              float dt,
              float out_min,
              float out_max)
{
    /* Validate inputs (assertions compile out in release builds) */
    assert(pid != NULL && "PID structure pointer cannot be NULL");
    assert(dt > 0.0f && "Sample time must be positive");
    assert(kp >= 0.0f && "Proportional gain must be non-negative");
    assert(ki >= 0.0f && "Integral gain must be non-negative");
    assert(kd >= 0.0f && "Derivative gain must be non-negative");
    assert(out_min < out_max && "Output min must be less than max");

    /* Store configuration parameters */
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;
    pid->out_min = out_min;
    pid->out_max = out_max;

    /* Initialize internal state to zero (fresh start) */
    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->derivative_filtered = 0.0f;

    /* Calculate default integrator limits based on output limits and Ki
     * This prevents integrator from accumulating beyond what's useful.
     * If Ki=0 (integral disabled), use output limits directly to avoid div-by-zero.
     */
    if (ki != 0.0f) {
        pid->integrator_min = out_min / ki;
        pid->integrator_max = out_max / ki;
    } else {
        /* If integral term disabled, integrator limits don't matter,
         * but set them to reasonable values anyway */
        pid->integrator_min = out_min;
        pid->integrator_max = out_max;
    }

    /* No derivative filtering by default */
    pid->derivative_lpf = 0.0f;
}

/**
 * @brief Initialize PID controller with advanced configuration
 *
 * See detailed documentation in pid.h
 *
 * Implementation notes:
 * - Provides full control over integrator limits (useful for asymmetric systems)
 * - Derivative filter coefficient is clamped to [0, 1] range
 * - All state variables initialized to zero
 * - Takes ~15-25 CPU cycles on typical embedded processors
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
                      float derivative_lpf)
{
    /* Validate inputs (assertions compile out in release builds) */
    assert(pid != NULL && "PID structure pointer cannot be NULL");
    assert(dt > 0.0f && "Sample time must be positive");
    assert(kp >= 0.0f && "Proportional gain must be non-negative");
    assert(ki >= 0.0f && "Integral gain must be non-negative");
    assert(kd >= 0.0f && "Derivative gain must be non-negative");
    assert(out_min < out_max && "Output min must be less than max");
    assert(integrator_min < integrator_max && "Integrator min must be less than max");

    /* Store configuration parameters */
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;
    pid->out_min = out_min;
    pid->out_max = out_max;

    /* Initialize internal state to zero (fresh start) */
    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->derivative_filtered = 0.0f;

    /* Use custom integrator limits provided by caller */
    pid->integrator_min = integrator_min;
    pid->integrator_max = integrator_max;

    /* Clamp derivative filter coefficient to valid range [0, 1]
     * 0 = no filtering (raw derivative)
     * 1 = maximum filtering (derivative response very slow)
     */
    pid->derivative_lpf = clamp(derivative_lpf, 0.0f, 1.0f);
}

/**
 * @brief Calculate PID control output
 *
 * See detailed documentation in pid.h
 *
 * Implementation algorithm:
 *
 * 1. Calculate error = setpoint - measurement
 *
 * 2. Proportional term:
 *    P = Kp × error
 *    Immediate response to current error
 *
 * 3. Integral term with anti-windup:
 *    integrator += error × dt
 *    integrator = clamp(integrator, integrator_min, integrator_max)  <-- Anti-windup
 *    I = Ki × integrator
 *    Eliminates steady-state error over time
 *
 * 4. Derivative term (on measurement, not error):
 *    derivative_raw = -(measurement - prev_measurement) / dt
 *    Note: Negative sign because we want to oppose changes in measurement
 *    If filtering enabled:
 *      derivative_filtered = α × derivative_filtered + (1-α) × derivative_raw
 *      derivative_raw = derivative_filtered
 *    D = Kd × derivative_raw
 *    Dampens oscillations and improves stability
 *
 * 5. Combine and clamp:
 *    output = P + I + D
 *    output = clamp(output, out_min, out_max)
 *
 * 6. Update state for next iteration:
 *    prev_error = error
 *    prev_measurement = measurement
 *
 * Performance: ~20-40 CPU cycles on ARM Cortex-M4
 */
float pid_compute(pid_t *pid, float setpoint, float measurement)
{
    /* Calculate error between desired and actual values */
    float error = setpoint - measurement;

    /*------------------------------------------------------------------------*/
    /* Proportional Term                                                      */
    /*------------------------------------------------------------------------*/
    float p = pid->kp * error;

    /*------------------------------------------------------------------------*/
    /* Integral Term with Anti-Windup                                        */
    /*------------------------------------------------------------------------*/
    /* Accumulate error over time (Riemann sum approximation) */
    pid->integrator += error * pid->dt;

    /* Clamp integrator to prevent excessive accumulation (anti-windup)
     * This is critical for:
     * - Preventing integrator from growing unbounded during saturation
     * - Faster recovery when error reduces
     * - More predictable transient response
     */
    pid->integrator = clamp(pid->integrator,
                            pid->integrator_min,
                            pid->integrator_max);

    /* Calculate integral contribution */
    float i = pid->ki * pid->integrator;

    /*------------------------------------------------------------------------*/
    /* Derivative Term (Derivative-on-Measurement)                           */
    /*------------------------------------------------------------------------*/
    /* Calculate rate of change of measurement (not error!)
     * Using derivative-on-measurement eliminates "derivative kick"
     * when setpoint changes suddenly.
     *
     * Negative sign explanation:
     * - If measurement is increasing, derivative_raw is negative
     * - This produces a negative D term (opposes the increase)
     * - This is the correct control action to slow down overshoot
     */
    float derivative_raw = -(measurement - pid->prev_measurement) / pid->dt;

    /* Apply optional low-pass filter to reduce noise amplification
     * This is an exponential moving average (EMA):
     * filtered = α × filtered_old + (1-α) × raw
     * where α = derivative_lpf (filter coefficient)
     *
     * Higher α = more filtering = slower response = less noise
     */
    if (pid->derivative_lpf > 0.0f) {
        pid->derivative_filtered = pid->derivative_filtered * pid->derivative_lpf +
                                  derivative_raw * (1.0f - pid->derivative_lpf);
        derivative_raw = pid->derivative_filtered;
    }

    /* Calculate derivative contribution */
    float d = pid->kd * derivative_raw;

    /*------------------------------------------------------------------------*/
    /* Output Calculation and Limiting                                       */
    /*------------------------------------------------------------------------*/
    /* Sum all three terms */
    float output = p + i + d;

    /* Clamp output to actuator limits
     * This is separate from integrator anti-windup and handles
     * the final output regardless of which term caused saturation
     */
    output = clamp(output, pid->out_min, pid->out_max);

    /*------------------------------------------------------------------------*/
    /* State Update                                                          */
    /*------------------------------------------------------------------------*/
    /* Store current values for next iteration */
    pid->prev_error = error;
    pid->prev_measurement = measurement;

    return output;
}

/**
 * @brief Reset PID controller internal state
 *
 * See detailed documentation in pid.h
 *
 * Implementation notes:
 * - Zeros all state variables (integrator, history, filtered values)
 * - Does NOT modify configuration (gains, limits, sample time)
 * - Equivalent to re-initializing with same parameters
 * - Takes ~5-10 CPU cycles
 * - Safe to call at any time, even during active control
 *
 * Use cases:
 * - Starting control after inactivity (motor was stopped)
 * - Large setpoint changes (avoid transient from old integrator state)
 * - Fault recovery (clear potentially invalid state)
 * - Mode switching (position control → velocity control)
 */
void pid_reset(pid_t *pid)
{
    pid->integrator = 0.0f;           /* Clear integral accumulation */
    pid->prev_error = 0.0f;           /* Clear error history */
    pid->prev_measurement = 0.0f;     /* Clear measurement history */
    pid->derivative_filtered = 0.0f;  /* Clear filtered derivative state */
}

/*============================================================================*/
/* END OF FILE                                                               */
/*============================================================================*/
