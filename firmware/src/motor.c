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

/**
 * @brief Current motor control output (last commanded duty cycle)
 *
 * Units: Normalized duty cycle
 * Range: -1.0 (full reverse) to +1.0 (full forward)
 *
 * This stores the most recent value set by motor_set_output()
 * and is used to update the motor model dynamics.
 */
static float current_output = 0.0f;

/* Model parameters */
static const float model_dt = 0.01f;      /* 10ms time step */
static const float model_tau = 0.2f;      /* 200ms time constant */
static const float model_gain = 5.0f;     /* Speed per unit input */
static const float model_alpha = model_dt / model_tau;  /* Response rate */

/*============================================================================*/
/* PUBLIC API IMPLEMENTATION                                                 */
/*============================================================================*/

/**
 * @brief Initialize motor hardware and simulation model
 *
 * See detailed documentation in motor.h
 *
 * Implementation notes (simulation):
 * - Resets motor speed to zero (stopped)
 * - Resets control output to zero (no commanded motion)
 * - No actual hardware initialization (this is simulation)
 *
 * For real hardware implementation:
 * - Initialize PWM peripheral (timer configuration, frequency, resolution)
 * - Configure direction control GPIOs as outputs
 * - Initialize encoder/tachometer interface (timer in encoder mode or GPIO interrupts)
 * - Ensure motor starts in safe stopped state
 * - Optionally enable motor driver (enable pin)
 */
void motor_init(void)
{
    current_speed = 0.0f;   /* Motor starts at rest */
    current_output = 0.0f;  /* No control output */
}

void motor_set_output(float duty_cycle)
{
    /* Clamp to [-1.0, 1.0] range */
    if (duty_cycle > 1.0f) duty_cycle = 1.0f;
    if (duty_cycle < -1.0f) duty_cycle = -1.0f;

    current_output = duty_cycle;
}

/**
 * @brief Get current motor speed measurement
 *
 * See detailed documentation in motor.h
 *
 * Implementation notes (simulation):
 * - Returns the simulated motor speed directly (perfect measurement)
 * - No noise, quantization, or measurement delay
 * - Unrealistically perfect feedback
 *
 * For real hardware implementation:
 * - Read encoder counter value from timer peripheral
 * - Calculate velocity from change in position:
 *   velocity = (position[n] - position[n-1]) / Δt
 * - Apply scaling factor to convert counts/sec to desired units (RPM, rad/s)
 * - Optionally apply filtering to reduce encoder noise:
 *   - Moving average filter (simple, adds delay)
 *   - Low-pass filter (better frequency response)
 *   - Or use PID derivative filtering (recommended)
 * - Handle counter overflow/wraparound (16-bit or 32-bit counters)
 *
 * Example calculation (quadrature encoder):
 * - Encoder resolution: 1000 counts/revolution (CPR)
 * - Time between measurements: 10ms
 * - Measured Δcounts: 50 counts in 10ms
 * - Velocity = (50 counts / 10ms) × (60000 ms/min) / (1000 counts/rev)
 *            = 300 RPM
 *
 * @return Current motor speed in arbitrary units (simulation)
 *         For hardware: typically RPM, rad/s, or counts/second
 */
float motor_get_speed(void)
{
    return current_speed;

    /*------------------------------------------------------------------------*/
    /* Real Hardware Implementation Would Be:                                */
    /*------------------------------------------------------------------------*/
    /*
     * static uint32_t prev_count = 0;
     * static uint32_t prev_time_us = 0;
     *
     * // Read current encoder count and timestamp
     * uint32_t current_count = TIM4->CNT;  // Encoder timer counter
     * uint32_t current_time_us = get_microseconds();
     *
     * // Calculate change in position and time
     * int32_t delta_counts = (int32_t)(current_count - prev_count);
     * uint32_t delta_time_us = current_time_us - prev_time_us;
     *
     * // Convert to RPM (example: 1000 CPR encoder)
     * float rpm = 0.0f;
     * if (delta_time_us > 0) {
     *     // rpm = (counts/time) × (60 sec/min) / (counts/rev)
     *     rpm = (delta_counts * 60000000.0f) / (delta_time_us * 1000.0f);
     * }
     *
     * // Update previous values
     * prev_count = current_count;
     * prev_time_us = current_time_us;
     *
     * return rpm;
     */
}

void motor_update(void)
{
    /* First-order linear dynamics: speed approaches target_speed */
    float target_speed = current_output * model_gain;
    current_speed += model_alpha * (target_speed - current_speed);
}
