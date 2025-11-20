/**
 * @file    motor.h
 * @brief   DC motor hardware abstraction layer for embedded control systems
 * @author  Onesmo Ogore
 * @date    November 17, 2025
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025 Onesmo Ogore
 * @license MIT License
 *
 * SPDX-License-Identifier: MIT
 *
 * @section DESCRIPTION
 *
 * This file provides a hardware abstraction layer (HAL) for DC motor control
 * in embedded systems. It abstracts the low-level hardware details (PWM, GPIO,
 * encoder/tachometer) into a simple, platform-agnostic API.
 *
 * The implementation supports:
 * - **Bidirectional control**: Forward and reverse motor direction
 * - **Variable speed**: PWM-based speed control with duty cycle
 * - **Speed feedback**: RPM measurement from encoder/tachometer
 * - **Hardware abstraction**: Platform-agnostic interface
 * - **Simple integration**: Easy to adapt to different MCU platforms
 *
 * @section HARDWARE
 *
 * Typical hardware configuration:
 * - **Motor driver**: H-bridge (L298N, DRV8833, TB6612FNG, etc.)
 * - **PWM output**: Timer-based PWM for speed control
 * - **Direction control**: GPIO pins for motor direction (optional)
 * - **Speed sensor**: Encoder, Hall effect sensor, or tachometer
 * - **MCU**: Any platform with PWM and GPIO (STM32, ESP32, Arduino, etc.)
 *
 * @section USAGE
 *
 * Basic motor control example:
 * @code
 * #include "motor.h"
 *
 * int main(void) {
 *     // Initialize motor hardware (PWM, GPIO, encoder)
 *     motor_init();
 *
 *     // Set motor to 75% forward speed
 *     motor_set_output(0.75f);
 *
 *     // Read current motor speed
 *     float current_rpm = motor_get_speed();
 *
 *     // Reverse motor at 50% speed
 *     motor_set_output(-0.50f);
 *
 *     // Stop motor
 *     motor_set_output(0.0f);
 *
 *     return 0;
 * }
 * @endcode
 *
 * Integration with PID controller:
 * @code
 * #include "motor.h"
 * #include "pid.h"
 *
 * pid_t speed_controller;
 *
 * void control_loop_init(void) {
 *     motor_init();
 *
 *     // Initialize PID for motor speed control
 *     pid_init(&speed_controller,
 *              1.0f,    // Kp
 *              0.5f,    // Ki
 *              0.1f,    // Kd
 *              0.01f,   // dt = 10ms
 *              -1.0f,   // Min output (full reverse)
 *              1.0f);   // Max output (full forward)
 * }
 *
 * void control_loop_update(void) {
 *     // Called every 10ms
 *     float target_rpm = 1000.0f;
 *     float current_rpm = motor_get_speed();
 *
 *     // PID computes control output in range [-1.0, 1.0]
 *     float motor_output = pid_compute(&speed_controller,
 *                                      target_rpm,
 *                                      current_rpm);
 *
 *     // Apply control output to motor
 *     motor_set_output(motor_output);
 * }
 * @endcode
 *
 * @section PORTING
 *
 * To port this motor HAL to your specific hardware:
 *
 * 1. **Implement motor_init()**: Initialize PWM timer, GPIO pins, encoder
 * 2. **Implement motor_set_output()**: Convert duty cycle to PWM settings
 * 3. **Implement motor_get_speed()**: Read encoder/tachometer and compute RPM
 *
 * Example platform-specific implementations:
 * - **STM32**: Use HAL_TIM functions for PWM, HAL_GPIO for direction
 * - **ESP32**: Use ledc functions for PWM, gpio functions for direction
 * - **Arduino**: Use analogWrite() for PWM, digitalWrite() for direction
 * - **RP2040**: Use pwm_set_gpio_level() for PWM control
 *
 * @section THREAD_SAFETY
 *
 * This implementation is NOT inherently thread-safe. If accessing motor
 * functions from multiple threads or interrupt contexts, implement external
 * synchronization (mutexes, critical sections, interrupt disabling).
 *
 * @note The current implementation is a **simulation model** for testing
 *       PID control algorithms. For real hardware, replace firmware/src/motor.c
 *       with platform-specific motor driver code.
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <stdint.h>    /**< Standard integer types */

/*============================================================================*/
/* PUBLIC API FUNCTIONS                                                      */
/*============================================================================*/

/**
 * @brief Initialize motor hardware and peripherals
 *
 * This function performs all necessary initialization for motor control:
 * - Configures PWM timer/channel for speed control
 * - Configures GPIO pins for direction control (if applicable)
 * - Initializes encoder/tachometer interface for speed measurement
 * - Resets motor to stopped state (0% duty cycle)
 *
 * This function should be called **once** during system initialization,
 * before any calls to motor_set_output() or motor_get_speed().
 *
 * Platform-specific implementation requirements:
 * - **PWM**: Configure timer with appropriate frequency (typically 20-50kHz)
 * - **GPIO**: Configure direction pins as outputs (if using H-bridge)
 * - **Encoder**: Configure timer in encoder mode or GPIO interrupts
 * - **Safety**: Ensure motor starts in stopped/safe state
 *
 * @return None
 *
 * @note This function must be called before any other motor functions
 * @note Calling this function multiple times is safe (idempotent)
 * @note Motor is guaranteed to be stopped after initialization
 *
 * @warning Ensure proper hardware connections before calling this function
 * @warning Verify motor driver voltage ratings match your motor specifications
 *
 * @par Example (STM32 platform):
 * @code
 * void motor_init(void) {
 *     // Initialize PWM timer (e.g., TIM3 Channel 1)
 *     HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
 *
 *     // Initialize direction GPIO pins
 *     HAL_GPIO_WritePin(MOTOR_DIR1_GPIO_Port, MOTOR_DIR1_Pin, GPIO_PIN_RESET);
 *     HAL_GPIO_WritePin(MOTOR_DIR2_GPIO_Port, MOTOR_DIR2_Pin, GPIO_PIN_RESET);
 *
 *     // Initialize encoder timer (e.g., TIM4 in encoder mode)
 *     HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
 *
 *     // Set initial duty cycle to 0 (motor stopped)
 *     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
 * }
 * @endcode
 *
 * @par Example (ESP32 platform):
 * @code
 * void motor_init(void) {
 *     // Configure LEDC for PWM (channel 0, 20kHz, 10-bit resolution)
 *     ledc_timer_config_t timer_conf = {
 *         .speed_mode = LEDC_LOW_SPEED_MODE,
 *         .duty_resolution = LEDC_TIMER_10_BIT,
 *         .timer_num = LEDC_TIMER_0,
 *         .freq_hz = 20000,
 *     };
 *     ledc_timer_config(&timer_conf);
 *
 *     ledc_channel_config_t channel_conf = {
 *         .channel = LEDC_CHANNEL_0,
 *         .gpio_num = MOTOR_PWM_GPIO,
 *         .duty = 0,  // Start stopped
 *     };
 *     ledc_channel_config(&channel_conf);
 *
 *     // Configure direction GPIOs
 *     gpio_set_direction(MOTOR_DIR1_GPIO, GPIO_MODE_OUTPUT);
 *     gpio_set_direction(MOTOR_DIR2_GPIO, GPIO_MODE_OUTPUT);
 * }
 * @endcode
 *
 * @see motor_set_output() to control motor speed and direction
 * @see motor_get_speed() to read current motor speed
 */
void motor_init(void);

/**
 * @brief Set motor output (speed and direction)
 *
 * Controls motor speed and direction using a normalized duty cycle value.
 * Positive values drive the motor forward, negative values drive reverse,
 * and zero stops the motor.
 *
 * The duty cycle controls the average voltage applied to the motor via
 * pulse-width modulation (PWM). Higher absolute values result in higher
 * motor speeds.
 *
 * Platform-specific implementation:
 * - Convert duty cycle to PWM compare value
 * - Set direction pins based on sign of duty
 * - Apply deadband/minimum duty if needed
 *
 * @param[in] duty  Normalized motor duty cycle
 *                  Range: -1.0 to +1.0
 *                  - **+1.0**: Full speed forward (100% duty cycle)
 *                  - **+0.5**: Half speed forward (50% duty cycle)
 *                  - **0.0**:  Motor stopped (0% duty cycle)
 *                  - **-0.5**: Half speed reverse (50% duty cycle)
 *                  - **-1.0**: Full speed reverse (100% duty cycle)
 *
 * @return None
 *
 * @note Values outside [-1.0, 1.0] are automatically clamped to valid range
 * @note Very small duty cycles (<5%) may not overcome motor friction
 * @note Calling with 0.0 provides active braking (if H-bridge supports it)
 *
 * @warning Rapid direction changes may cause current spikes - consider ramping
 * @warning Ensure motor driver current rating exceeds motor stall current
 * @warning Operating motor at 100% duty continuously may cause overheating
 *
 * @par Example (simple speed control):
 * @code
 * motor_init();
 *
 * // Ramp motor from 0% to 100% forward
 * for (float duty = 0.0f; duty <= 1.0f; duty += 0.1f) {
 *     motor_set_output(duty);
 *     delay_ms(100);
 * }
 *
 * // Stop motor
 * motor_set_output(0.0f);
 *
 * // Reverse at 75% speed
 * motor_set_output(-0.75f);
 * @endcode
 *
 * @par Example (platform-specific implementation - STM32):
 * @code
 * void motor_set_output(float duty) {
 *     // Clamp to valid range
 *     if (duty > 1.0f) duty = 1.0f;
 *     if (duty < -1.0f) duty = -1.0f;
 *
 *     // Set direction based on sign
 *     if (duty >= 0.0f) {
 *         HAL_GPIO_WritePin(DIR1_Port, DIR1_Pin, GPIO_PIN_SET);   // Forward
 *         HAL_GPIO_WritePin(DIR2_Port, DIR2_Pin, GPIO_PIN_RESET);
 *     } else {
 *         HAL_GPIO_WritePin(DIR1_Port, DIR1_Pin, GPIO_PIN_RESET); // Reverse
 *         HAL_GPIO_WritePin(DIR2_Port, DIR2_Pin, GPIO_PIN_SET);
 *         duty = -duty;  // Make positive for PWM calculation
 *     }
 *
 *     // Convert to PWM compare value (assuming 1000 = 100%)
 *     uint16_t pwm_value = (uint16_t)(duty * 1000.0f);
 *     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_value);
 * }
 * @endcode
 *
 * @see motor_init() must be called first
 * @see motor_get_speed() to measure resulting motor speed
 */
void motor_set_output(float duty);

/**
 * @brief Get current motor speed measurement
 *
 * Reads the motor speed from the encoder/tachometer interface and returns
 * the measured speed. The units and scale depend on the platform-specific
 * implementation (RPM, normalized units, encoder counts, etc.).
 *
 * Common measurement methods:
 * - **Quadrature encoder**: Count pulses over time period, convert to RPM
 * - **Hall effect sensor**: Measure pulse frequency, convert to RPM
 * - **Back-EMF sensing**: Measure motor voltage during off-time (advanced)
 * - **Simulation**: Simple first-order motor model (current implementation)
 *
 * Platform-specific implementation:
 * - Read encoder counter value
 * - Calculate velocity from position change
 * - Apply calibration/scaling factors
 * - Filter noise if necessary
 *
 * @return Current motor speed
 *         - **Positive**: Motor rotating forward
 *         - **Negative**: Motor rotating reverse
 *         - **Zero**: Motor stopped
 *         - **Units**: Platform-dependent (RPM, rad/s, or normalized)
 *
 * @note Return value units depend on your specific implementation
 * @note Typical range: 0-5000 RPM for small DC motors
 * @note Speed measurement may have latency depending on encoder resolution
 * @note Consider filtering the returned value if noisy (see PID derivative_lpf)
 *
 * @warning Speed reading accuracy depends on encoder resolution and sample rate
 * @warning At very low speeds, measurement may be unreliable or quantized
 * @warning Ensure encoder connections are secure to avoid erratic readings
 *
 * @par Example (basic usage):
 * @code
 * motor_init();
 * motor_set_output(0.8f);  // 80% forward
 *
 * delay_ms(100);  // Allow motor to accelerate
 *
 * float speed = motor_get_speed();
 * printf("Motor speed: %.1f RPM\n", speed);
 * @endcode
 *
 * @par Example (closed-loop control with PID):
 * @code
 * pid_t motor_controller;
 * pid_init(&motor_controller, 1.0f, 0.5f, 0.1f, 0.01f, -1.0f, 1.0f);
 *
 * // Control loop (called every 10ms)
 * void timer_callback(void) {
 *     float target_speed = 1000.0f;  // Target: 1000 RPM
 *     float actual_speed = motor_get_speed();
 *
 *     float control = pid_compute(&motor_controller, target_speed, actual_speed);
 *     motor_set_output(control);
 * }
 * @endcode
 *
 * @par Example (platform-specific implementation - STM32 encoder):
 * @code
 * float motor_get_speed(void) {
 *     static uint32_t prev_count = 0;
 *     static uint32_t prev_time_ms = 0;
 *
 *     // Read current encoder count
 *     uint32_t current_count = __HAL_TIM_GET_COUNTER(&htim4);
 *     uint32_t current_time_ms = HAL_GetTick();
 *
 *     // Calculate change in position and time
 *     int32_t delta_counts = (int32_t)(current_count - prev_count);
 *     uint32_t delta_time_ms = current_time_ms - prev_time_ms;
 *
 *     // Convert to RPM (assuming 1000 counts/rev encoder)
 *     float rpm = 0.0f;
 *     if (delta_time_ms > 0) {
 *         rpm = (delta_counts / 1000.0f) * (60000.0f / delta_time_ms);
 *     }
 *
 *     prev_count = current_count;
 *     prev_time_ms = current_time_ms;
 *
 *     return rpm;
 * }
 * @endcode
 *
 * @see motor_init() must be called first
 * @see motor_set_output() to control motor based on speed feedback
 */
float motor_get_speed(void);

/*============================================================================*/
/* END OF PUBLIC API                                                         */
/*============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H_ */
