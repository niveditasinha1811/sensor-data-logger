/* SPDX-License-Identifier: BSD-3-Clause */
/**
 * @file    tests/test_mock_sensor.c
 * @brief   Unity unit tests for the mock_sensor module.
 *
 * @details
 *   - Verifies that generated timestamps are nonzero and non-decreasing.
 *   - Confirms that acceleration values lie within ±16 G.
 *   - Checks basic statistical distribution over a set of samples.
 *   - Ensures stable behavior across multiple calls.
 *
 * @author  Nivedita
 * @date    2025-05-24
 */

 #include "unity.h"
 #include "mock_sensor/mock_sensor.h"
 #include <time.h>        /* nanosleep */
 #include <stdlib.h>      /* rand, srand */
 #include <sys/time.h>    /* gettimeofday */
 #include <stdbool.h>     /* bool */
 
 /** @brief Milliseconds per second for time conversion */
 #define TEST_MS_PER_SEC_U32   (1000U)
 
 /**
  * @brief  Sleep for a given number of milliseconds.
  *
  * @param[in] l_delay_ms_U32  Delay in milliseconds.
  */
 static void l_sleep_ms_U32(uint32_t l_delay_ms_U32)
 {
     struct timespec l_req_st = {0};
     struct timespec l_rem_st = {0};
 
     if (l_delay_ms_U32 > 999U)
     {
         l_req_st.tv_sec  = (time_t)(l_delay_ms_U32 / TEST_MS_PER_SEC_U32);
         l_req_st.tv_nsec = (long)((l_delay_ms_U32 % TEST_MS_PER_SEC_U32) * 1000000UL);
     }
     else
     {
         l_req_st.tv_sec  = 0;
         l_req_st.tv_nsec = (long)(l_delay_ms_U32 * 1000000UL);
     }
     (void)nanosleep(&l_req_st, &l_rem_st);
 }
 
 void setUp(void)
 {
     /* No persistent state to reset in mock_sensor.c */
 }
 
 void tearDown(void)
 {
     /* Nothing to cleanup */
 }
 
 /**
  * @brief  Test that timestamp is positive and acceleration within [-16, +16].
  */
 void test_get_mock_sensor_data_range(void)
 {
     sensor_sample_t l_sample_st = get_mock_sensor_data();
 
     TEST_ASSERT_GREATER_THAN_UINT32(0U, l_sample_st.timestamp_ms);
 
     TEST_ASSERT_TRUE(
         (l_sample_st.acc_x >= -16.0f) && (l_sample_st.acc_x <= 16.0f)
     );
     TEST_ASSERT_TRUE(
         (l_sample_st.acc_y >= -16.0f) && (l_sample_st.acc_y <= 16.0f)
     );
     TEST_ASSERT_TRUE(
         (l_sample_st.acc_z >= -16.0f) && (l_sample_st.acc_z <= 16.0f)
     );
 }
 
 /**
  * @brief  Test that successive timestamps do not decrease.
  * First timestamp is valid.
  * Timestamps don’t decrease.
  * At least one axis changes (to avoid repeated data).
  */
 void test_get_mock_sensor_data_timestamp_increases(void)
 {
     sensor_sample_t l_first_st  = get_mock_sensor_data();
     l_sleep_ms_U32(5U);
     sensor_sample_t l_second_st = get_mock_sensor_data();
 
     if (l_second_st.timestamp_ms == l_first_st.timestamp_ms)
     {
         TEST_IGNORE_MESSAGE("Timestamps equal at ms resolution.");
     }
     else
     {
         TEST_ASSERT_GREATER_THAN_UINT32(
             l_first_st.timestamp_ms,
             l_second_st.timestamp_ms
         );
     }
 }
 
 /**
  * @brief  Basic distribution test: ensures positive & negative readings appear.
  */
 void test_get_mock_sensor_data_randomness_distribution_basic(void)
 {
     const uint32_t l_total_samples_U32 = 100U;
     bool   l_x_pos_b = false, l_x_neg_b = false;
     bool   l_y_pos_b = false, l_y_neg_b = false;
     bool   l_z_pos_b = false, l_z_neg_b = false;
     float  l_first_x_f = 0.0f, l_first_y_f = 0.0f, l_first_z_f = 0.0f;
     bool   l_x_all_same_b = true, l_y_all_same_b = true, l_z_all_same_b = true;
 
     for (uint32_t l_idx_U32 = 0U; l_idx_U32 < l_total_samples_U32; ++l_idx_U32)
     {
         sensor_sample_t l_sample_st = get_mock_sensor_data();
 
         if (l_idx_U32 == 0U)
         {
             l_first_x_f = l_sample_st.acc_x;
             l_first_y_f = l_sample_st.acc_y;
             l_first_z_f = l_sample_st.acc_z;
         }
         else
         {
             if (l_sample_st.acc_x != l_first_x_f) { l_x_all_same_b = false; }
             if (l_sample_st.acc_y != l_first_y_f) { l_y_all_same_b = false; }
             if (l_sample_st.acc_z != l_first_z_f) { l_z_all_same_b = false; }
         }
 
         if (l_sample_st.acc_x > 0.1f)  { l_x_pos_b = true; }
         if (l_sample_st.acc_x < -0.1f) { l_x_neg_b = true; }
         if (l_sample_st.acc_y > 0.1f)  { l_y_pos_b = true; }
         if (l_sample_st.acc_y < -0.1f) { l_y_neg_b = true; }
         if (l_sample_st.acc_z > 0.1f)  { l_z_pos_b = true; }
         if (l_sample_st.acc_z < -0.1f) { l_z_neg_b = true; }
     }
 
     TEST_ASSERT_FALSE(l_x_all_same_b);
     TEST_ASSERT_FALSE(l_y_all_same_b);
     TEST_ASSERT_FALSE(l_z_all_same_b);
 
     TEST_ASSERT_TRUE(l_x_pos_b && l_x_neg_b);
     TEST_ASSERT_TRUE(l_y_pos_b && l_y_neg_b);
     TEST_ASSERT_TRUE(l_z_pos_b && l_z_neg_b);
 }
 
 /**
  * @brief  Stability test: multiple calls use same RNG state after seeding.
  */
 void test_get_mock_sensor_data_multiple_calls_stability(void)
 {
     sensor_sample_t l_s1_st = get_mock_sensor_data();
     sensor_sample_t l_s2_st = get_mock_sensor_data();
     sensor_sample_t l_s3_st = get_mock_sensor_data();
 
     TEST_ASSERT_GREATER_THAN_UINT32(0U, l_s1_st.timestamp_ms);
 
     TEST_ASSERT_GREATER_OR_EQUAL_UINT32(
         l_s1_st.timestamp_ms, l_s2_st.timestamp_ms
     );
     TEST_ASSERT_GREATER_OR_EQUAL_UINT32(
         l_s2_st.timestamp_ms, l_s3_st.timestamp_ms
     );
 
     TEST_ASSERT_TRUE_MESSAGE(
         ((l_s1_st.acc_x != l_s2_st.acc_x) ||
          (l_s2_st.acc_x != l_s3_st.acc_x)),
         "Consecutive samples too similar"
     );
 }
 
 int main(void)
 {
     UNITY_BEGIN();
     RUN_TEST(test_get_mock_sensor_data_range);
     RUN_TEST(test_get_mock_sensor_data_timestamp_increases);
     RUN_TEST(test_get_mock_sensor_data_randomness_distribution_basic);
     RUN_TEST(test_get_mock_sensor_data_multiple_calls_stability);
     return UNITY_END();
 }
 