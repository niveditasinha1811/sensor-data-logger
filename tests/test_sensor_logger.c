/* SPDX-License-Identifier: BSD-3-Clause */
/**
 * @file    test_sensor_logger.c
 * @brief   Unity unit tests for sensor_logger module.
 */

#include "unity.h"
#include "sensor_logger/sensor_logger.h"
#include <stdio.h>
#include <string.h>

/* Buffer to capture printf output via fmemopen */
static char output_buffer[4096];
static FILE *orig_stdout;

void setUp(void)
{
    /* Redirect stdout to our buffer */
    orig_stdout = stdout;
    stdout = fmemopen(output_buffer, sizeof(output_buffer), "w");
    TEST_ASSERT_NOT_NULL(stdout);
}

void tearDown(void)
{
    /* Restore stdout and null-terminate buffer */
    fflush(stdout);
    stdout = orig_stdout;
    output_buffer[sizeof(output_buffer)-1] = '\0';
}

void test_log_null_sample(void)
{
    TEST_ASSERT_EQUAL_INT(-1, log_sensor_data(NULL));
}

void test_init_and_print_empty(void)
{
    init_sensor_logger();
    int chars = print_log();
    TEST_ASSERT_EQUAL_INT(0, chars);
}

void test_single_sample(void)
{
    sensor_sample_t sample = {
        .timestamp_ms = 1000u,
        .acc_x = 1.0f,
        .acc_y = 2.0f,
        .acc_z = 3.0f
    };
    init_sensor_logger();
    TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&sample));

    int chars = print_log();
    TEST_ASSERT_GREATER_THAN_INT(0, chars);
    TEST_ASSERT_NOT_NULL(strstr(output_buffer, "1000,1.000000,2.000000,3.000000"));
}

void test_wrap_around(void)
{
    init_sensor_logger();
    /* Log more than buffer size to force wrap-around */
    for (int i = 0; i < (int)SENSOR_LOGGER_BUFFER_SIZE + 2; ++i)
    {
        sensor_sample_t s = {
            .timestamp_ms = (uint32_t)i,
            .acc_x = (float)i,
            .acc_y = 0.0f,
            .acc_z = 0.0f
        };
        TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s));
    }

    int chars = print_log();
    TEST_ASSERT_GREATER_THAN_INT(0, chars);

    /* Count lines in output_buffer */
    int lines = 0;
    for (char *p = output_buffer; *p; ++p)
    {
        if (*p == '\n') ++lines;
    }
    TEST_ASSERT_EQUAL_INT((int)SENSOR_LOGGER_BUFFER_SIZE, lines);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_log_null_sample);
    RUN_TEST(test_init_and_print_empty);
    RUN_TEST(test_single_sample);
    RUN_TEST(test_wrap_around);
    return UNITY_END();
}
