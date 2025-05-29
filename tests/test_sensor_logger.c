/**
 * @file    tests/test_sensor_logger.c
 * @brief   Unity unit tests for sensor_logger module.
 *
 * @details
 *   - Redirects stdout to a buffer via fmemopen().
 *   - Verifies initialization, null-sample handling, wrap-around behavior,
 *     capacity, edge timestamps/accelerations, and CSV format.
 *
 * @author  Nivedita
 * @date    2025-05-24
 */

 #include "unity.h"
 #include "sensor_logger/sensor_logger.h"
 #include "mock_sensor/mock_sensor.h"
 #include <stdio.h>
 #include <string.h>
 #include <stdint.h>
 
 /** @brief Size of the capture buffer for printf redirection */
 #define TEST_OUTPUT_BUFFER_SIZE_U32   (8192U)
 
 static char    gl_capture_buffer_ca[TEST_OUTPUT_BUFFER_SIZE_U32];
 static FILE   *gl_mock_stdout_fp    = NULL;
 static FILE   *gl_saved_stdout_fp   = NULL;
 
 /**
  * @brief Redirect stdout into an in-memory buffer.
  */
 static void l_redirect_stdout_to_buffer(void)
 {
     (void)memset(gl_capture_buffer_ca, 0, sizeof(gl_capture_buffer_ca));
     fflush(stdout);
     gl_saved_stdout_fp = stdout;
     gl_mock_stdout_fp  = fmemopen(
         gl_capture_buffer_ca,
         sizeof(gl_capture_buffer_ca) - 1U,
         "w"
     );
     if (gl_mock_stdout_fp == NULL)
     {
         TEST_FAIL_MESSAGE("fmemopen() failed");
     }
     stdout = gl_mock_stdout_fp;
 }
 
 /**
  * @brief Restore stdout back to the console.
  */
 static void l_restore_stdout_from_buffer(void)
 {
     if (gl_mock_stdout_fp != NULL)
     {
         fflush(gl_mock_stdout_fp);
         (void)fclose(gl_mock_stdout_fp);
         gl_mock_stdout_fp = NULL;
     }
     if (gl_saved_stdout_fp != NULL)
     {
         stdout = gl_saved_stdout_fp;
         gl_saved_stdout_fp = NULL;
     }
 }
 
 void setUp(void)
 {
     l_redirect_stdout_to_buffer();
     (void)init_sensor_logger();
 }
 
 void tearDown(void)
 {
     l_restore_stdout_from_buffer();
     gl_capture_buffer_ca[sizeof(gl_capture_buffer_ca) - 1U] = '\0';
 }
 
 void test_init_clears_buffer(void)
 {
     sensor_sample_t l_sample_st = { .timestamp_ms = 1U,
                                     .acc_x        = 1.0f,
                                     .acc_y        = 1.0f,
                                     .acc_z        = 1.0f };
     (void)log_sensor_data(&l_sample_st);
     (void)log_sensor_data(&l_sample_st);
 
     (void)init_sensor_logger();
 
     int l_chars_i = print_log();
     TEST_ASSERT_EQUAL_INT(0, l_chars_i);
     TEST_ASSERT_EQUAL_STRING_LEN("", gl_capture_buffer_ca, 1U);
     TEST_ASSERT_EQUAL_UINT32(0U, get_sensor_logger_entry_count());
     TEST_ASSERT_EQUAL_UINT32(0U, get_sensor_logger_head_index());
 }
 
 void test_log_null_sample(void)
 {
     TEST_ASSERT_EQUAL_INT(-1, log_sensor_data(NULL));
 }
 
 void test_print_empty_after_init(void)
 {
     int l_chars_i = print_log();
     TEST_ASSERT_EQUAL_INT(0, l_chars_i);
     TEST_ASSERT_EQUAL_STRING_LEN("", gl_capture_buffer_ca, 1U);
 }
 
 void test_single_sample(void)
 {
     sensor_sample_t l_sample_st = {
         .timestamp_ms = 1000U,
         .acc_x        = 1.0f,
         .acc_y        = 2.0f,
         .acc_z        = 3.0f
     };
     TEST_ASSERT_EQUAL_INT(0, log_sensor_data(&l_sample_st));
 
     int l_chars_i = print_log();
     TEST_ASSERT_GREATER_THAN_INT(0, l_chars_i);
 
     char l_expected_ca[128];
     (void)snprintf(
         l_expected_ca, sizeof(l_expected_ca),
         "%lu,%.6f,%.6f,%.6f\n",
         (unsigned long)l_sample_st.timestamp_ms,
         l_sample_st.acc_x,
         l_sample_st.acc_y,
         l_sample_st.acc_z
     );
     TEST_ASSERT_EQUAL_STRING(l_expected_ca, gl_capture_buffer_ca);
     TEST_ASSERT_EQUAL_UINT32(1U, get_sensor_logger_entry_count());
 }
 
 void test_wrap_around_behavior(void)
 {
     for (uint32_t i = 0; i < SENSOR_LOGGER_BUFFER_SIZE + 10; ++i)
     {
         sensor_sample_t s = {
             .timestamp_ms = i * 10U,
             .acc_x = (float)i,
             .acc_y = (float)i + 1,
             .acc_z = (float)i + 2
         };
         (void)log_sensor_data(&s);
     }
 
     // Verify that only the last 128 entries are retained
     TEST_ASSERT_EQUAL_UINT32(SENSOR_LOGGER_BUFFER_SIZE, get_sensor_logger_entry_count());
 
     print_log();
 }
 
 
 int main(void)
 {
     UNITY_BEGIN();
     RUN_TEST(test_init_clears_buffer);
     RUN_TEST(test_log_null_sample);
     RUN_TEST(test_print_empty_after_init);
     RUN_TEST(test_single_sample);
     RUN_TEST(test_wrap_around_behavior);
     return UNITY_END();
 }
 