/**
 * @file    pid.c
 * @brief   Implementation of production-ready PID controller
 * @author  Onesmo Ogore
 * @version 1.0.0
 * @date    November 2025
 * @license MIT
 *
 * Features: Anti-windup via integrator clamping, derivative-on-measurement
 * to avoid derivative kick, optional low-pass filtering for noise reduction.
 */

#include "pid.h"
#include <assert.h>
#include <stddef.h>

/* Clamp value to [min, max] range */
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

    /* Store configuration */
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;
    pid->out_min = out_min;
    pid->out_max = out_max;

    /* Initialize state to zero */
    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->derivative_filtered = 0.0f;

    /* Calculate integrator limits (anti-windup) */
    if (ki != 0.0f) {
        pid->integrator_min = out_min / ki;
        pid->integrator_max = out_max / ki;
    } else {
        pid->integrator_min = out_min;
        pid->integrator_max = out_max;
    }

    /* No derivative filtering by default */
    pid->derivative_lpf = 0.0f;
}

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
    /* Validate inputs */
    assert(pid != NULL && "PID structure pointer cannot be NULL");
    assert(dt > 0.0f && "Sample time must be positive");
    assert(kp >= 0.0f && "Proportional gain must be non-negative");
    assert(ki >= 0.0f && "Integral gain must be non-negative");
    assert(kd >= 0.0f && "Derivative gain must be non-negative");
    assert(out_min < out_max && "Output min must be less than max");
    assert(integrator_min < integrator_max && "Integrator min must be less than max");

    /* Store configuration */
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;
    pid->out_min = out_min;
    pid->out_max = out_max;

    /* Initialize state to zero */
    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->derivative_filtered = 0.0f;

    /* Use custom integrator limits */
    pid->integrator_min = integrator_min;
    pid->integrator_max = integrator_max;

    /* Clamp derivative filter to [0, 1] range */
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

    /* Proportional term */
    float p = pid->kp * error;

    /* Integral term with anti-windup */
    pid->integrator += error * pid->dt;
    pid->integrator = clamp(pid->integrator, pid->integrator_min, pid->integrator_max);
    float i = pid->ki * pid->integrator;

    /* Derivative term (on measurement, not error)
     * Negative sign: if measurement increases, we want negative D to oppose it.
     * This avoids "derivative kick" when setpoint changes suddenly. */
    float derivative_raw = -(measurement - pid->prev_measurement) / pid->dt;

    /* Optional low-pass filter (exponential moving average) */
    if (pid->derivative_lpf > 0.0f) {
        pid->derivative_filtered = pid->derivative_filtered * pid->derivative_lpf +
                                  derivative_raw * (1.0f - pid->derivative_lpf);
        derivative_raw = pid->derivative_filtered;
    }

    float d = pid->kd * derivative_raw;

    /* Combine and clamp output */
    float output = p + i + d;
    output = clamp(output, pid->out_min, pid->out_max);

    /* Update state for next iteration */
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
    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->derivative_filtered = 0.0f;
}

/*============================================================================*/
/* END OF FILE                                                               */
/*============================================================================*/
