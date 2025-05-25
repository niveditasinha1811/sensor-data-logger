/* SPDX-License-Identifier: BSD-3-Clause */
/**
 * @file    test_mock_sensor.c
 * @brief   Unity unit tests for mock_sensor module.
 */

 #include "unity.h"
 #include "mock_sensor/mock_sensor.h"
 #include "sensor_logger/sensor_logger.h"
 
 void setUp(void)  { /* no-op */ }
 void tearDown(void) { /* no-op */ }
 
 void test_get_mock_sensor_data_range(void)
 {
     sensor_sample_t s = get_mock_sensor_data();
     /* Timestamp should be non-zero */
     TEST_ASSERT_GREATER_THAN_UINT32(0u, s.timestamp_ms);
     /* Each axis in ±16 G */
     TEST_ASSERT_TRUE_MESSAGE((s.acc_x >= -16.0f && s.acc_x <= 16.0f),
                              "acc_x out of range");
     TEST_ASSERT_TRUE_MESSAGE((s.acc_y >= -16.0f && s.acc_y <= 16.0f),
                              "acc_y out of range");
     TEST_ASSERT_TRUE_MESSAGE((s.acc_z >= -16.0f && s.acc_z <= 16.0f),
                              "acc_z out of range");
 }
 
 int main(void)
 {
     UNITY_BEGIN();
     RUN_TEST(test_get_mock_sensor_data_range);
     return UNITY_END();
 }
 