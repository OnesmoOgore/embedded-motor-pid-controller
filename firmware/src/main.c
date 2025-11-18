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
int main(void)
{
    motor_init();

    pid_t pid;
    pid_init(&pid,
             0.8f,   // Kp
             0.3f,   // Ki
             0.05f,  // Kd
             0.01f,  // dt (s)
             -1.0f,  // out_min
             1.0f);  // out_max

    float setpoint = 3.0f; // desired speed (arbitrary units)

    // simple "control loop" running in a for loop instead of a timer
    for (int step = 0; step < 1000; ++step)
    {
        float speed = motor_get_speed();
        float u = pid_compute(&pid, setpoint, speed);

        motor_set_output(u);

        // For now, just print to stdout (desktop build)
        printf("%d,%.4f,%.4f,%.4f\n", step, setpoint, speed, u);
    }

    return 0;
}
