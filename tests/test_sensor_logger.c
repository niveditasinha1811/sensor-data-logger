/* SPDX-License-Identifier: BSD-3-Clause */
/**
 * @file    test_sensor_logger.c
 * @brief   Unity unit tests for sensor_logger module.
 */

 
 #include "unity.h"
 #include "sensor_logger/sensor_logger.h" // This now includes the declarations for helper functions
 #include <stdio.h>
 #include <string.h>
 #include <stdint.h> // For UINT32_MAX
 
 /* Buffer to capture printf output via fmemopen */
 #define OUTPUT_BUFFER_SIZE 8192 // Increased size for more extensive logging tests
 static char output_buffer[OUTPUT_BUFFER_SIZE];
 static FILE *mock_stdout;
 static FILE *saved_stdout; // To save the original stdout
 
 // Helper to redirect stdout for capturing LOG_PRINTF output
 void redirect_stdout_to_buffer(void) {
     memset(output_buffer, 0, sizeof(output_buffer));
     fflush(stdout); // Ensure real stdout is flushed before redirecting
     saved_stdout = stdout; // Save the current stdout
     mock_stdout = fmemopen(output_buffer, sizeof(output_buffer) - 1, "w"); // -1 for null terminator space
     if (mock_stdout == NULL) {
         perror("fmemopen failed in redirect_stdout_to_buffer");
         TEST_FAIL_MESSAGE("fmemopen failed for stdout redirection.");
         return;
     }
     stdout = mock_stdout; // Redirect stdout
 }
 
 void restore_stdout_from_buffer(void) {
     if (mock_stdout) {
         fflush(mock_stdout); // Ensure all data is written to the buffer
         fclose(mock_stdout);
         mock_stdout = NULL;
     }
     if (saved_stdout) {
         stdout = saved_stdout; // Restore original stdout
         saved_stdout = NULL;
     }
     // output_buffer is already null-terminated by fmemopen behavior or by tearDown
 }
 
 
 void setUp(void)
 {
     redirect_stdout_to_buffer();
     init_sensor_logger(); // Ensure a clean state for each test
 }
 
 void tearDown(void)
 {
     restore_stdout_from_buffer();
     output_buffer[sizeof(output_buffer)-1] = '\0'; // Ensure null termination for safety
 }
 
 void test_init_clears_previously_filled_buffer(void) {
     sensor_sample_t sample = { .timestamp_ms = 1, .acc_x = 1.0f, .acc_y = 1.0f, .acc_z = 1.0f };
     log_sensor_data(&sample);
     log_sensor_data(&sample);
 
     init_sensor_logger(); // Re-initialize
 
     int chars = print_log();
     TEST_ASSERT_EQUAL_INT(0, chars);
     TEST_ASSERT_EQUAL_STRING_LEN("", output_buffer, 1);
     TEST_ASSERT_EQUAL_UINT32(0, get_sensor_logger_entry_count());
     TEST_ASSERT_EQUAL_UINT32(0, get_sensor_logger_head_index());
 }
 
 void test_log_null_sample(void)
 {
     TEST_ASSERT_EQUAL_INT(-1, log_sensor_data(NULL));
 }
 
 void test_init_and_print_empty(void)
 {
     // setUp already calls init_sensor_logger()
     int chars = print_log();
     TEST_ASSERT_EQUAL_INT(0, chars);
     TEST_ASSERT_EQUAL_STRING_LEN("", output_buffer, 1);
 }
 
 void test_single_sample(void)
 {
     sensor_sample_t sample = {
         .timestamp_ms = 1000u,
         .acc_x = 1.0f,
         .acc_y = 2.0f,
         .acc_z = 3.0f
     };
     TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&sample));
 
     int chars = print_log();
     TEST_ASSERT_GREATER_THAN_INT(0, chars);
     char expected_output[100];
     sprintf(expected_output, "%lu,%.6f,%.6f,%.6f\n", (unsigned long)sample.timestamp_ms, sample.acc_x, sample.acc_y, sample.acc_z);
     TEST_ASSERT_EQUAL_STRING(expected_output, output_buffer);
     TEST_ASSERT_EQUAL_UINT32(1, get_sensor_logger_entry_count());
 }
 
 void test_wrap_around(void)
 {
     // setUp already calls init_sensor_logger()
     for (int i = 0; i < (int)SENSOR_LOGGER_BUFFER_SIZE + 5; ++i)
     {
         sensor_sample_t s = {
             .timestamp_ms = (uint32_t)i,
             .acc_x = (float)i,
             .acc_y = 0.0f,
             .acc_z = 0.0f
         };
         TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s));
     }
 
     print_log();
 
     int lines = 0;
     for (char *p = output_buffer; *p; ++p)
     {
         if (*p == '\n') lines++;
     }
     TEST_ASSERT_EQUAL_INT_MESSAGE(SENSOR_LOGGER_BUFFER_SIZE, lines, "Should only print buffer size after wrap around");
 
     char expected_first_sample_str[100];
     sprintf(expected_first_sample_str, "%d,%.6f", 5, 5.0f);
     TEST_ASSERT_NOT_NULL_MESSAGE(strstr(output_buffer, expected_first_sample_str), "Oldest samples not overwritten correctly.");
 
     TEST_ASSERT_EQUAL_UINT32(SENSOR_LOGGER_BUFFER_SIZE, get_sensor_logger_entry_count());
     TEST_ASSERT_EQUAL_UINT32(5 % SENSOR_LOGGER_BUFFER_SIZE, get_sensor_logger_head_index());
 }
 
 void test_log_sensor_data_fills_buffer_to_capacity(void) {
     for (unsigned int i = 0; i < SENSOR_LOGGER_BUFFER_SIZE; ++i) {
         sensor_sample_t s = { .timestamp_ms = i, .acc_x = (float)i, .acc_y = 0.1f * i, .acc_z = -0.1f * i };
         TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s));
     }
 
     TEST_ASSERT_EQUAL_UINT32(SENSOR_LOGGER_BUFFER_SIZE, get_sensor_logger_entry_count());
     TEST_ASSERT_EQUAL_UINT32(0, get_sensor_logger_head_index()); // Should wrap around to 0
 
     print_log();
     int lines = 0;
     for (char *p = output_buffer; *p; ++p) {
         if (*p == '\n') lines++;
     }
     TEST_ASSERT_EQUAL_INT(SENSOR_LOGGER_BUFFER_SIZE, lines);
 
     char expected_first_sample_str[100];
     sprintf(expected_first_sample_str, "%u,%.6f", 0, 0.0f);
     TEST_ASSERT_NOT_NULL(strstr(output_buffer, expected_first_sample_str));
 
     char expected_last_sample_str[100];
     sprintf(expected_last_sample_str, "%u,%.6f", (unsigned int)SENSOR_LOGGER_BUFFER_SIZE - 1, (float)(SENSOR_LOGGER_BUFFER_SIZE - 1));
     TEST_ASSERT_NOT_NULL(strstr(output_buffer, expected_last_sample_str));
 }
 
 void test_log_sensor_data_timestamp_extremes(void) {
     sensor_sample_t s_zero = { .timestamp_ms = 0, .acc_x = 1.0f, .acc_y = 2.0f, .acc_z = 3.0f };
     sensor_sample_t s_max = { .timestamp_ms = UINT32_MAX, .acc_x = -1.0f, .acc_y = -2.0f, .acc_z = -3.0f };
 
     TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s_zero));
     TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s_max));
 
     print_log();
 
     char expected_output_zero[100];
     sprintf(expected_output_zero, "%lu,%.6f,%.6f,%.6f\n", (unsigned long)s_zero.timestamp_ms, s_zero.acc_x, s_zero.acc_y, s_zero.acc_z);
     TEST_ASSERT_NOT_NULL_MESSAGE(strstr(output_buffer, expected_output_zero), "Timestamp 0 not logged/printed correctly.");
 
     char expected_output_max[100];
     sprintf(expected_output_max, "%lu,%.6f,%.6f,%.6f\n", (unsigned long)s_max.timestamp_ms, s_max.acc_x, s_max.acc_y, s_max.acc_z);
     TEST_ASSERT_NOT_NULL_MESSAGE(strstr(output_buffer, expected_output_max), "Timestamp UINT32_MAX not logged/printed correctly.");
 }
 
 void test_log_sensor_data_acceleration_extremes(void) {
     sensor_sample_t s_min_acc = { .timestamp_ms = 123, .acc_x = -16.0f, .acc_y = -16.0f, .acc_z = -16.0f };
     sensor_sample_t s_max_acc = { .timestamp_ms = 456, .acc_x = 16.0f, .acc_y = 16.0f, .acc_z = 16.0f };
     sensor_sample_t s_zero_acc = { .timestamp_ms = 789, .acc_x = 0.0f, .acc_y = 0.0f, .acc_z = 0.0f };
 
     TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s_min_acc));
     TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s_max_acc));
     TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s_zero_acc));
 
     print_log();
 
     char expected_output_min[100];
     sprintf(expected_output_min, "%lu,%.6f,%.6f,%.6f\n", (unsigned long)s_min_acc.timestamp_ms, s_min_acc.acc_x, s_min_acc.acc_y, s_min_acc.acc_z);
     TEST_ASSERT_NOT_NULL_MESSAGE(strstr(output_buffer, expected_output_min), "Min accelerations not logged/printed correctly.");
     // Other checks for max and zero can be added similarly
 }
 
 void test_log_sensor_data_multiple_wraparounds(void) {
     unsigned int total_samples_to_log = SENSOR_LOGGER_BUFFER_SIZE * 3 + 7;
     for (unsigned int i = 0; i < total_samples_to_log; ++i) {
         sensor_sample_t s = { .timestamp_ms = i, .acc_x = (float)i, .acc_y = 0.5f * i, .acc_z = -0.5f * i };
         TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s));
     }
 
     TEST_ASSERT_EQUAL_UINT32(SENSOR_LOGGER_BUFFER_SIZE, get_sensor_logger_entry_count());
     TEST_ASSERT_EQUAL_UINT32((total_samples_to_log % SENSOR_LOGGER_BUFFER_SIZE), get_sensor_logger_head_index());
 
     print_log();
     int lines = 0;
     for (char *p = output_buffer; *p; ++p) {
         if (*p == '\n') lines++;
     }
     TEST_ASSERT_EQUAL_INT(SENSOR_LOGGER_BUFFER_SIZE, lines);
 
     char expected_oldest_str[100];
     unsigned int expected_oldest_ts = total_samples_to_log - SENSOR_LOGGER_BUFFER_SIZE;
     sprintf(expected_oldest_str, "%u,%.6f", expected_oldest_ts, (float)expected_oldest_ts);
     TEST_ASSERT_NOT_NULL_MESSAGE(strstr(output_buffer, expected_oldest_str), "Oldest sample after multiple wraps is incorrect.");
 
     char expected_newest_str[100];
     unsigned int expected_newest_ts = total_samples_to_log - 1;
     sprintf(expected_newest_str, "%u,%.6f", expected_newest_ts, (float)expected_newest_ts);
     TEST_ASSERT_NOT_NULL_MESSAGE(strstr(output_buffer, expected_newest_str), "Newest sample after multiple wraps is incorrect.");
 }
 
 
 void test_print_log_output_format_precision(void) {
     sensor_sample_t sample = { .timestamp_ms = 12345, .acc_x = 1.23456789f, .acc_y = -0.987654321f, .acc_z = 123.0f };
     TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&sample));
     print_log();
 
     char expected_output[200];
     sprintf(expected_output, "%lu,1.234568,-0.987654,123.000000\n", (unsigned long)sample.timestamp_ms);
     TEST_ASSERT_EQUAL_STRING(expected_output, output_buffer);
 }
 
 void test_print_log_partially_filled_buffer(void) {
     int num_to_log = SENSOR_LOGGER_BUFFER_SIZE / 2;
     if (num_to_log == 0) num_to_log = 1;
 
     for (int i = 0; i < num_to_log; ++i) {
         sensor_sample_t s = { .timestamp_ms = (uint32_t)i, .acc_x = (float)i, .acc_y = 0.0f, .acc_z = 0.0f };
         TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&s));
     }
 
     TEST_ASSERT_EQUAL_UINT32(num_to_log, get_sensor_logger_entry_count());
     print_log();
     int lines = 0;
     for (char *p = output_buffer; *p; ++p) {
         if (*p == '\n') lines++;
     }
     TEST_ASSERT_EQUAL_INT(num_to_log, lines);
 
     char expected_first_str[100];
     sprintf(expected_first_str, "%d,%.6f", 0, 0.0f);
     TEST_ASSERT_NOT_NULL(strstr(output_buffer, expected_first_str));
 
     char expected_last_str[100];
     sprintf(expected_last_str, "%d,%.6f", num_to_log - 1, (float)(num_to_log - 1));
     TEST_ASSERT_NOT_NULL(strstr(output_buffer, expected_last_str));
 }
 
 void test_print_log_after_one_entry_then_init(void) {
     sensor_sample_t sample = { .timestamp_ms = 1, .acc_x = 1.0f, .acc_y = 1.0f, .acc_z = 1.0f };
     log_sensor_data(&sample);
     init_sensor_logger(); // This should clear entry_count
     int chars = print_log();
     TEST_ASSERT_EQUAL_INT(0, chars);
     TEST_ASSERT_EQUAL_STRING_LEN("", output_buffer, 1);
 }
 
 
 int main(void)
 {
     UNITY_BEGIN();
     RUN_TEST(test_init_clears_previously_filled_buffer);
     RUN_TEST(test_log_null_sample);
     RUN_TEST(test_init_and_print_empty);
     RUN_TEST(test_single_sample);
     RUN_TEST(test_wrap_around);
     RUN_TEST(test_log_sensor_data_fills_buffer_to_capacity);
     RUN_TEST(test_log_sensor_data_timestamp_extremes);
     RUN_TEST(test_log_sensor_data_acceleration_extremes);
     RUN_TEST(test_log_sensor_data_multiple_wraparounds);
     RUN_TEST(test_print_log_output_format_precision);
     RUN_TEST(test_print_log_partially_filled_buffer);
     RUN_TEST(test_print_log_after_one_entry_then_init);
     return UNITY_END();
 }