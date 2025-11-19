/*
 * @file    pid.c
 * @author  Onesmo Ogore
 * @date    11/17/2025
 * @brief   Implementation of the Proportional-Integral-Derivative (PID) algorithm.
 * 
 * SPDX-License-Identifier: MIT
 *
 * @section DESCRIPTION
 * This file provides functions for initializing the PID controller structure, 
 * calculating the control output based on feedback, and resetting integral windup.
 */

// Includes
#include "pid.h"

// PID code

static float clamp(float value, float min, float max)
{
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

void pid_init(pid_t *pid,
              float kp,
              float ki,
              float kd,
              float dt,
              float out_min,
              float out_max)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;

    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->derivative_filtered = 0.0f;
    pid->out_min = out_min;
    pid->out_max = out_max;

    // Default integrator limits based on output limits
    pid->integrator_min = out_min / (ki != 0.0f ? ki : 1.0f);
    pid->integrator_max = out_max / (ki != 0.0f ? ki : 1.0f);

    // No derivative filtering by default
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
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;

    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->derivative_filtered = 0.0f;
    pid->out_min = out_min;
    pid->out_max = out_max;

    pid->integrator_min = integrator_min;
    pid->integrator_max = integrator_max;

    // Clamp derivative filter coefficient to valid range
    pid->derivative_lpf = clamp(derivative_lpf, 0.0f, 1.0f);
}

float pid_compute(pid_t *pid, float setpoint, float measurement)
{
    float error = setpoint - measurement;

    // Proportional term
    float p = pid->kp * error;

    // Integral term with anti-windup
    pid->integrator += error * pid->dt;
    // Clamp integrator to prevent windup
    pid->integrator = clamp(pid->integrator, pid->integrator_min, pid->integrator_max);
    float i = pid->ki * pid->integrator;

    // Derivative term (derivative-on-measurement to avoid derivative kick)
    float derivative_raw = -(measurement - pid->prev_measurement) / pid->dt;

    // Apply low-pass filter to derivative if configured
    if (pid->derivative_lpf > 0.0f) {
        pid->derivative_filtered = pid->derivative_filtered * pid->derivative_lpf +
                                  derivative_raw * (1.0f - pid->derivative_lpf);
        derivative_raw = pid->derivative_filtered;
    }

    float d = pid->kd * derivative_raw;

    // Calculate output
    float output = p + i + d;

    // Clamp output to limits
    output = clamp(output, pid->out_min, pid->out_max);

    // Update state
    pid->prev_error = error;
    pid->prev_measurement = measurement;

    return output;
}

void pid_reset(pid_t *pid)
{
    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->derivative_filtered = 0.0f;
}
