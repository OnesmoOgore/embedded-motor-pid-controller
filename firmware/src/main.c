/**
 * @file    main.c
 * @brief   PID motor controller demo application
 * @author  Onesmo Ogore
 * @version 1.0.0
 * @date    November 2025
 * @license MIT
 *
 * Demonstrates PID-based motor speed control. Runs as desktop simulation
 * with CSV output for analysis. For embedded use, replace with timer
 * interrupt and hardware-specific motor functions.
 */

#include "motor.h"
#include "pid.h"
#include <stdio.h>

/* Configuration */
#define NUM_ITERATIONS  500     /* Simulation steps */
#define SAMPLE_TIME_MS  10      /* Control loop period (10ms = 100Hz) */
#define SAMPLE_TIME_S   (SAMPLE_TIME_MS / 1000.0f)

/* PID gains (tuned for simulation model) */
#define PID_KP   0.8f   /* Proportional gain */
#define PID_KI   0.3f   /* Integral gain */
#define PID_KD   0.05f  /* Derivative gain */

/* Control limits */
#define OUT_MIN  -1.0f  /* Full reverse */
#define OUT_MAX   1.0f  /* Full forward */

/* Target speed */
#define SETPOINT  3.0f  /* Desired motor speed */

int main(void)
{
    pid_t motor_pid;

    /* Initialize motor and PID controller */
    motor_init();
    pid_init(&motor_pid, PID_KP, PID_KI, PID_KD, SAMPLE_TIME_S, OUT_MIN, OUT_MAX);

    /* CSV header for simulation output */
    printf("step,setpoint,measurement,output\n");

    /* Control loop */
    for (int step = 0; step < NUM_ITERATIONS; step++) {
        /* Read current motor speed */
        float measurement = motor_get_speed();

        /* Compute PID control output */
        float output = pid_compute(&motor_pid, SETPOINT, measurement);

        /* Apply control output to motor */
        motor_set_output(output);

        /* Update motor simulation */
        motor_update();

        /* Log data (CSV format) */
        printf("%d,%.4f,%.4f,%.4f\n", step, SETPOINT, measurement, output);
    }

    return 0;
}
