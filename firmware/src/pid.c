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
void pid_init(pid_t *pid, float kp, float ki, float kd, float dt, float out_min, float out_max)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->dt = dt;
    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
    pid->out_min = out_min;
    pid->out_max = out_max;
}

float pid_compute(pid_t *pid, float setpoint, float measurement)
{
    float error = setpoint - measurement;

    // Proportional
    float p = pid->kp * error;

    // Integral
    pid->integrator += error * pid->dt;
    float i = pid->ki * pid->integrator;

    // Derivative (simple backward difference)
    float derivative = (error - pid->prev_error) / pid->dt;
    float d = pid->kd * derivative;

    float output = p + i + d;

    // Clamp output
    if (output > pid->out_max)
        output = pid->out_max;
    else if (output < pid->out_min)
        output = pid->out_min;

    pid->prev_error = error;

    return output;
}

void pid_reset(pid_t *pid)
{
    pid->integrator = 0.0f;
    pid->prev_error = 0.0f;
}
