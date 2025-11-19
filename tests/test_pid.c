/*
 * @file    test_pid.c
 * @author  Onesmo Ogore
 * @date    11/19/2025
 * @brief   Unit tests for PID controller implementation
 *
 * SPDX-License-Identifier: MIT
 */

#include "Unity/src/unity.h"
#include "../firmware/include/pid.h"
#include <math.h>

void setUp(void)
{
    // This is run before each test
}

void tearDown(void)
{
    // This is run after each test
}

/* Test: Initialization sets all parameters correctly */
void test_pid_init_sets_parameters(void)
{
    pid_t pid;
    pid_init(&pid, 1.0f, 0.5f, 0.1f, 0.01f, -100.0f, 100.0f);

    TEST_ASSERT_EQUAL_FLOAT(1.0f, pid.kp);
    TEST_ASSERT_EQUAL_FLOAT(0.5f, pid.ki);
    TEST_ASSERT_EQUAL_FLOAT(0.1f, pid.kd);
    TEST_ASSERT_EQUAL_FLOAT(0.01f, pid.dt);
    TEST_ASSERT_EQUAL_FLOAT(-100.0f, pid.out_min);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, pid.out_max);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, pid.integrator);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, pid.prev_error);
}

/* Test: Pure proportional control (Ki=0, Kd=0) */
void test_pid_proportional_only(void)
{
    pid_t pid;
    pid_init(&pid, 2.0f, 0.0f, 0.0f, 0.01f, -100.0f, 100.0f);

    float output = pid_compute(&pid, 10.0f, 5.0f);
    // Error = 10 - 5 = 5
    // P = 2.0 * 5 = 10
    TEST_ASSERT_EQUAL_FLOAT(10.0f, output);
}

/* Test: Pure integral control (Kp=0, Kd=0) */
void test_pid_integral_only(void)
{
    pid_t pid;
    pid_init(&pid, 0.0f, 1.0f, 0.0f, 0.1f, -100.0f, 100.0f);

    // First call: error = 10, integrator = 10 * 0.1 = 1.0
    float output1 = pid_compute(&pid, 10.0f, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, output1);

    // Second call: error = 10, integrator = 1.0 + 10*0.1 = 2.0
    float output2 = pid_compute(&pid, 10.0f, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, output2);
}

/* Test: Pure derivative control (Kp=0, Ki=0) */
void test_pid_derivative_only(void)
{
    pid_t pid;
    pid_init(&pid, 0.0f, 0.0f, 1.0f, 0.1f, -100.0f, 100.0f);

    // First call: prev_measurement = 0, current measurement = 0
    // derivative-on-measurement = -(0 - 0) / 0.1 = 0
    float output1 = pid_compute(&pid, 10.0f, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, output1);

    // Second call: measurement changes from 0 to 5
    // derivative = -(5 - 0) / 0.1 = -50
    // D = 1.0 * -50 = -50
    float output2 = pid_compute(&pid, 10.0f, 5.0f);
    TEST_ASSERT_EQUAL_FLOAT(-50.0f, output2);
}

/* Test: Output clamping to maximum limit */
void test_pid_output_clamp_max(void)
{
    pid_t pid;
    pid_init(&pid, 10.0f, 0.0f, 0.0f, 0.01f, -50.0f, 50.0f);

    // Error = 100 - 0 = 100, P = 10 * 100 = 1000
    // Should clamp to 50
    float output = pid_compute(&pid, 100.0f, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(50.0f, output);
}

/* Test: Output clamping to minimum limit */
void test_pid_output_clamp_min(void)
{
    pid_t pid;
    pid_init(&pid, 10.0f, 0.0f, 0.0f, 0.01f, -50.0f, 50.0f);

    // Error = -100 - 0 = -100, P = 10 * -100 = -1000
    // Should clamp to -50
    float output = pid_compute(&pid, -100.0f, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(-50.0f, output);
}

/* Test: Combined PID terms */
void test_pid_combined_terms(void)
{
    pid_t pid;
    pid_init(&pid, 1.0f, 0.5f, 0.1f, 0.1f, -100.0f, 100.0f);

    // First call: error = 10, measurement changes from 0 to 0
    // P = 1.0 * 10 = 10
    // I = 0.5 * (10 * 0.1) = 0.5
    // D = 0.1 * (-(0 - 0) / 0.1) = 0
    // Total = 10 + 0.5 + 0 = 10.5
    float output = pid_compute(&pid, 10.0f, 0.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.5f, output);
}

/* Test: Reset clears integrator and previous error */
void test_pid_reset(void)
{
    pid_t pid;
    pid_init(&pid, 1.0f, 1.0f, 1.0f, 0.1f, -100.0f, 100.0f);

    // Run a few iterations to build up integrator
    pid_compute(&pid, 10.0f, 0.0f);
    pid_compute(&pid, 10.0f, 0.0f);

    // Verify integrator has accumulated
    TEST_ASSERT_NOT_EQUAL(0.0f, pid.integrator);
    TEST_ASSERT_NOT_EQUAL(0.0f, pid.prev_error);

    // Reset
    pid_reset(&pid);

    TEST_ASSERT_EQUAL_FLOAT(0.0f, pid.integrator);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, pid.prev_error);
}

/* Test: Zero gains produce zero output */
void test_pid_zero_gains(void)
{
    pid_t pid;
    pid_init(&pid, 0.0f, 0.0f, 0.0f, 0.1f, -100.0f, 100.0f);

    float output = pid_compute(&pid, 100.0f, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, output);
}

/* Test: Negative error handling */
void test_pid_negative_error(void)
{
    pid_t pid;
    pid_init(&pid, 2.0f, 0.0f, 0.0f, 0.01f, -100.0f, 100.0f);

    // Error = 5 - 15 = -10
    // P = 2.0 * -10 = -20
    float output = pid_compute(&pid, 5.0f, 15.0f);
    TEST_ASSERT_EQUAL_FLOAT(-20.0f, output);
}

/* Test: No derivative kick on setpoint change (derivative-on-measurement) */
void test_pid_derivative_kick(void)
{
    pid_t pid;
    pid_init(&pid, 0.0f, 0.0f, 1.0f, 0.1f, -1000.0f, 1000.0f);

    // Steady state at setpoint 0
    pid_compute(&pid, 0.0f, 0.0f);

    // Sudden setpoint change (measurement doesn't change)
    // Derivative-on-measurement: measurement changes from 0 to 0
    // D = 1.0 * (-(0 - 0) / 0.1) = 0
    // This demonstrates NO derivative kick with derivative-on-measurement
    float output = pid_compute(&pid, 100.0f, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, output);
}

/* Test: Anti-windup prevents excessive integrator accumulation */
void test_pid_integral_accumulation(void)
{
    pid_t pid;
    pid_init(&pid, 0.0f, 1.0f, 0.0f, 0.1f, -10.0f, 10.0f);

    // Run with large error that would saturate output
    for (int i = 0; i < 100; i++) {
        pid_compute(&pid, 100.0f, 0.0f);
    }

    // With proper anti-windup, integrator should be clamped
    // integrator_max = 10.0 / 1.0 = 10.0
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, pid.integrator);

    // Verify integrator doesn't exceed limits
    TEST_ASSERT_LESS_OR_EQUAL(10.1f, pid.integrator);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_pid_init_sets_parameters);
    RUN_TEST(test_pid_proportional_only);
    RUN_TEST(test_pid_integral_only);
    RUN_TEST(test_pid_derivative_only);
    RUN_TEST(test_pid_output_clamp_max);
    RUN_TEST(test_pid_output_clamp_min);
    RUN_TEST(test_pid_combined_terms);
    RUN_TEST(test_pid_reset);
    RUN_TEST(test_pid_zero_gains);
    RUN_TEST(test_pid_negative_error);
    RUN_TEST(test_pid_derivative_kick);
    RUN_TEST(test_pid_integral_accumulation);

    return UNITY_END();
}
