/*
 * @file    motor.c
 * @author  Onesmo Ogore
 * @date    11/17/2025
 * @brief   Source file for the DC motor control module.
 * 
 * SPDX-License-Identifier: MIT
 *
 * @section DESCRIPTION
 * This file provides the implementation for initializing, controlling speed, 
 * and reading encoder feedback from the motor hardware.
 */

// Includes
#include "motor.h"

// Motor code
// For now, this is just a stub. On a real MCU, this would wrap PWM, timers, etc.

// Very simple first-order "plant" model for desktop tests.
// Not physically accurate, but good enough for demonstrating the loop.

static float current_speed = 0.0f;
static float current_output = 0.0f;

// pseudo "time step" for the model (seconds)
static const float model_dt = 0.01f;
// how quickly speed follows output
static const float model_gain = 5.0f;

void motor_init(void)
{
    current_speed = 0.0f;
    current_output = 0.0f;
}

void motor_set_output(float duty)
{
    if (duty > 1.0f) duty = 1.0f;
    if (duty < -1.0f) duty = -1.0f;
    current_output = duty;

    // crude model: speed moves toward output * model_gain
    float target_speed = current_output * model_gain;
    float alpha = model_dt; // "response speed"

    current_speed += alpha * (target_speed - current_speed);
}

float motor_get_speed(void)
{
    return current_speed;
}
