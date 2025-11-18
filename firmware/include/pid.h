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
    float kp;
    float ki;
    float kd;

    /* Control loop period (seconds) */
    float dt;          

    /* Control limits */
    float out_min;
    float out_max;
    
    /* Controller internal state (persistent memory) */
    float integrator;
    float prev_error;
    float setpoint;

} pid_t;


// --- Function Prototypes (The Module API) ---

/**
 * @brief  Initializes a PID controller instance with specific gains.
 * @param  pid Pointer to the PIDController_t structure to initialize.
 * @param  kp Proportional gain.
 * @param  ki Integral gain.
 * @param  kd Derivative gain.
 * @param  dt Control loop period (seconds).
 * @param  min Minimum output limit.
 * @param  max Maximum output limit.
 */
void pid_init(pid_t *pid, float kp, float ki, float kd,
              float dt, float min, float max);

/**
 * @brief  Calculates the next control output based on the current feedback.
 * @param  pid Pointer to the active PID structure.
 * @param  setpoint The current process target to achieve.
 * @param  measurement The current process feedback (e.g., motor speed).
 * @retval The calculated control output value.
 */
float pid_compute(pid_t *pid, float setpoint, float measurement);

/**
 * @brief  Resets the integral sum of the PID controller to zero.
 * @param  pid Pointer to the active PID structure.
 */
void pid_reset(pid_t *pid);

#endif /* PID_H */
