/**
 * @file    motor.h
 * @brief   Simple DC motor simulation for testing PID control
 * @author  Onesmo Ogore
 * @version 1.0.0
 * @date    November 2025
 * @license MIT
 *
 * This is a simulation model for testing PID algorithms. For real hardware,
 * replace motor.c with platform-specific PWM/GPIO/encoder code.
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initialize motor simulation
 *
 * Resets motor state to zero speed and zero output.
 */
void motor_init(void);

/**
 * @brief Set motor control output
 *
 * @param duty_cycle Control output (-1.0 to 1.0)
 *                   Negative = reverse, Positive = forward, 0 = stop
 */
void motor_set_output(float duty_cycle);

/**
 * @brief Get current motor speed
 *
 * @return Simulated motor speed (arbitrary units)
 */
float motor_get_speed(void);

/**
 * @brief Update motor simulation (call periodically)
 *
 * Advances the motor model by one time step. Should be called at
 * regular intervals matching the configured sample time.
 */
void motor_update(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H_ */
