/*
 * @file    motor.h
 * @author  Onesmo Ogore
 * @date    11/17/2025
 * @brief   Header file for the DC motor control module definitions.
 * 
 * SPDX-License-Identifier: MIT
 *
 * @section DESCRIPTION
 * This file contains the function prototypes, constants, and data types 
 * required to initialize and control the motor hardware peripherals.
 */

#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

void motor_init(void);
void motor_set_output(float duty);    // -1.0 to +1.0 for direction and magnitude
float motor_get_speed(void);          // e.g. RPM or normalized units

#endif // MOTOR_H
