/*
 * @file    main.c
 * @author  Onesmo Ogore
 * @date    11/17/2025
 * @version 1.0
 * @brief   Source file for the main.c control module.
 * 
 * @section LICENSE
 * This project is licensed under the MIT License.
 * See the [LICENSE](LICENSE) file for the full terms.
 *
 * @section DESCRIPTION
 * This file initializes system peripherals (GPIO, Timers, UART), 
 * sets up the tasks/threads (or main superloop), 
 * and starts the primary control loop for the motor PID controller.
 */

// Includes
#include "motor.h"
#include "pid.h"
#include <stdio.h>

// Main application code

