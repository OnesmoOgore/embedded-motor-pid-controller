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
#define SAMPLE_TIME     0.01f   /* Control loop period: 10ms = 100Hz */

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
    pid_init(&motor_pid, PID_KP, PID_KI, PID_KD, SAMPLE_TIME, OUT_MIN, OUT_MAX);

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

    /*------------------------------------------------------------------------*/
    /* Shutdown Phase (simulation only)                                     */
    /*------------------------------------------------------------------------*/

    /* For simulation: Program exits normally
     * For embedded: Typically never reaches here (infinite loop above)
     *
     * If shutdown is needed (embedded):
     * - Stop motor safely (ramp down, then disable)
     * - Save configuration to non-volatile memory
     * - Enter low-power mode
     */

    return 0;  /* Success (simulation mode only) */

    /*------------------------------------------------------------------------*/
    /* Example Embedded Implementation (Timer Interrupt)                    */
    /*------------------------------------------------------------------------*/
    /*
     * // Global PID instance (accessible from interrupt)
     * pid_t g_motor_pid;
     * float g_setpoint = 1000.0f;  // Target RPM
     *
     * void system_init(void) {
     *     motor_init();
     *     pid_init(&g_motor_pid, 1.0f, 0.5f, 0.1f, 0.01f, -100.0f, 100.0f);
     *
     *     // Configure timer for 10ms periodic interrupt
     *     TIM2_Init(100);  // 100Hz = 10ms period
     *     TIM2_EnableInterrupt();
     * }
     *
     * // Timer interrupt - called every 10ms
     * void TIM2_IRQHandler(void) {
     *     float speed = motor_get_speed();
     *     float output = pid_compute(&g_motor_pid, g_setpoint, speed);
     *     motor_set_output(output);
     *
     *     TIM2_ClearInterruptFlag();
     * }
     *
     * int main(void) {
     *     system_init();
     *     while (1) {
     *         // Main loop handles non-time-critical tasks:
     *         // - User interface
     *         // - Communication
     *         // - Data logging
     *         // - Fault monitoring
     *     }
     * }
     */
}

/*============================================================================*/
/* END OF FILE                                                               */
/*============================================================================*/
