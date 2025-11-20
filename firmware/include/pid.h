/**
 * @file    pid.h
 * @brief   Production-ready PID controller for embedded systems
 * @author  Onesmo Ogore
 * @version 1.0.0
 * @date    November 2025
 * @license MIT
 *
 * Features: Anti-windup protection, derivative-on-measurement,
 * derivative filtering, reentrant design, platform-agnostic C99.
 */

#ifndef PID_H_
#define PID_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief PID Controller instance structure
 *
 * Contains all configuration and state for a single PID loop.
 * Supports multiple independent instances. Do not modify members
 * directly - use the API functions.
 */
typedef struct {
    /* Configuration (set during initialization) */
    float kp;                  /**< Proportional gain (0.1-10.0) */
    float ki;                  /**< Integral gain (0.0-5.0, 0 to disable) */
    float kd;                  /**< Derivative gain (0.0-1.0, 0 to disable) */
    float dt;                  /**< Sample time in seconds (must match loop period) */
    float out_min;             /**< Minimum output limit */
    float out_max;             /**< Maximum output limit */
    float integrator_min;      /**< Min integrator limit (anti-windup) */
    float integrator_max;      /**< Max integrator limit (anti-windup) */
    float derivative_lpf;      /**< Derivative filter coeff (0.0-1.0, 0=no filter) */

    /* Internal state (modified during operation) */
    float integrator;          /**< Integral accumulator */
    float prev_error;          /**< Previous error (for future use) */
    float prev_measurement;    /**< Previous measurement (for derivative) */
    float derivative_filtered; /**< Filtered derivative value */
} pid_t;

/**
 * @brief Initialize PID controller with standard configuration
 *
 * Integrator limits are automatically calculated. No derivative filtering.
 *
 * @param pid      Pointer to PID structure
 * @param kp       Proportional gain
 * @param ki       Integral gain (0 to disable)
 * @param kd       Derivative gain (0 to disable)
 * @param dt       Sample time in seconds (must match loop period)
 * @param out_min  Minimum output limit
 * @param out_max  Maximum output limit
 */
void pid_init(pid_t *pid,
              float kp,
              float ki,
              float kd,
              float dt,
              float out_min,
              float out_max);

/**
 * @brief Initialize PID controller with advanced options
 *
 * Provides custom integrator limits and derivative filtering.
 *
 * @param pid             Pointer to PID structure
 * @param kp              Proportional gain
 * @param ki              Integral gain
 * @param kd              Derivative gain
 * @param dt              Sample time in seconds
 * @param out_min         Minimum output limit
 * @param out_max         Maximum output limit
 * @param integrator_min  Min integrator limit (typically out_min/ki)
 * @param integrator_max  Max integrator limit (typically out_max/ki)
 * @param derivative_lpf  Derivative filter (0.0=none, 0.7-0.9=recommended)
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
 * Must be called periodically at the rate specified by dt.
 * Uses derivative-on-measurement to avoid derivative kick.
 *
 * @param pid         Pointer to initialized PID structure
 * @param setpoint    Target value
 * @param measurement Current measured value
 * @return Control output clamped to [out_min, out_max]
 */
float pid_compute(pid_t *pid, float setpoint, float measurement);

/**
 * @brief Reset PID controller internal state
 *
 * Clears integrator, previous values, and filtered derivative.
 * Preserves configuration (gains, limits, sample time).
 *
 * @param pid Pointer to PID structure
 */
void pid_reset(pid_t *pid);

#ifdef __cplusplus
}
#endif

#endif /* PID_H_ */
