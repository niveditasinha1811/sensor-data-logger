/* SPDX-License-Identifier: BSD-3-Clause */
/**
 * @file    mock_sensor.h
 * @brief   Public API for generating mock sensor data.
 *
 * Provides a function to return a sensor_sample_t populated
 * with pseudo-random accelerometer values in ±16 G and the
 * current epoch timestamp in milliseconds.
 */

 #ifndef MOCK_SENSOR_H
 #define MOCK_SENSOR_H
 
 #include "sensor_logger/sensor_logger.h"  /**< For sensor_sample_t */
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief   Return a mock sensor sample.
  *
  * Timestamp is set via time.h (ms resolution); acc_x, acc_y, acc_z
  * are uniform random floats in the range [-16.0, +16.0].
  *
  * @return  A fully populated sensor_sample_t.
  */
 sensor_sample_t get_mock_sensor_data(void);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* MOCK_SENSOR_H */
 