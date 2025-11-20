/**
 * @file    motor.c
 * @brief   Simple first-order DC motor simulation model
 * @author  Onesmo Ogore
 * @version 1.0.0
 * @date    November 2025
 * @license MIT
 *
 * Implements a linearized first-order motor model for PID testing:
 *   speed[n+1] = speed[n] + alpha * (gain * input - speed[n])
 *
 * For real hardware, replace with platform-specific PWM/encoder code.
 */

#include "motor.h"

/* Simulation state */
static float current_speed = 0.0f;
static float current_output = 0.0f;

/* Model parameters */
static const float model_dt = 0.01f;      /* 10ms time step */
static const float model_tau = 0.2f;      /* 200ms time constant */
static const float model_gain = 5.0f;     /* Speed per unit input */
static const float model_alpha = model_dt / model_tau;  /* Response rate */

void motor_init(void)
{
    current_speed = 0.0f;
    current_output = 0.0f;
}

void motor_set_output(float duty_cycle)
{
    /* Clamp to [-1.0, 1.0] range */
    if (duty_cycle > 1.0f) duty_cycle = 1.0f;
    if (duty_cycle < -1.0f) duty_cycle = -1.0f;

    current_output = duty_cycle;
}

float motor_get_speed(void)
{
    return current_speed;
}

void motor_update(void)
{
    /* First-order linear dynamics: speed approaches target_speed */
    float target_speed = current_output * model_gain;
    current_speed += model_alpha * (target_speed - current_speed);
}
