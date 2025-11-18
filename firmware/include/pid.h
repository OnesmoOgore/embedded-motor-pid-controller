/*
 * @file    pid.h
 * @author  Onesmo Ogore
 * @date    11/17/2025
 * @brief   Header file for the PID control module definitions.
 * 
 * SPDX-License-Identifier: MIT
 *
 * @section DESCRIPTION
 * This file defines the data structures, constants, and function prototypes 
 * required to use the PID control loop algorithm within the application.
 */
#ifndef PID_H_ /* The include guard start line */
#define PID_H_

// --- Includes ---
#include <stdint.h> // Use standard integer types like float or double based on MCU capabilities

// --- Data Types and Structures ---

/**
 * @brief  PID Controller structure instance definition.
 * 
 * This structure holds the state and configuration of a single PID loop.
 */
typedef struct {
    /* PID Constants */
    float Kp;
    float Ki;
    float Kd;
    
    /* Control limits */
    float output_min;
    float output_max;
    
    /* Controller internal state (persistent memory) */
    float integral_sum;
    float prev_error;
    float setpoint;

} PIDController_t;


// --- Function Prototypes (The Module API) ---

/**
 * @brief  Initializes a PID controller instance with specific gains.
 * @param  pid_instance Pointer to the PIDController_t structure to initialize.
 * @param  Kp Proportional gain.
 * @param  Ki Integral gain.
 * @param  Kd Derivative gain.
 * @param  min Minimum output limit.
 * @param  max Maximum output limit.
 */
void PID_Init(PIDController_t *pid_instance, 
              float Kp, float Ki, float Kd, 
              float min, float max);

/**
 * @brief  Calculates the next control output based on the current feedback.
 * @param  pid_instance Pointer to the active PID structure.
 * @param  measured_value The current process feedback (e.g., motor speed).
 * @retval The calculated control output value.
 */
float PID_Calculate(PIDController_t *pid_instance, float measured_value);

/**
 * @brief  Resets the integral sum of the PID controller to zero.
 * @param  pid_instance Pointer to the active PID structure.
 */
void PID_ResetIntegral(PIDController_t *pid_instance);


#endif /* PID_H_ end of include */
