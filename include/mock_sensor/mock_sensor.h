/**
 * @file    include/mock_sensor/mock_sensor.h
 * @brief   Public API for mock sensor data generation.
 *
 * Provides a single function to obtain pseudo‐random accelerometer data
 * (±16 G) with a timestamp in epoch milliseconds.
 *
 * @author  Nivedita
 * @date    2025-05-24
 */

 #ifndef MOCK_SENSOR_H
 #define MOCK_SENSOR_H
 
 #include <stdint.h>
 #include "sensor_logger/sensor_logger.h"  /**< For sensor_sample_t */
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * @brief   Generate one mock sensor sample.
  *
  * On first call, seeds the RNG from the system time.  
  * Returns a sensor_sample_t with:
  *   - timestamp_ms: current epoch time in ms  
  *   - acc_x, acc_y, acc_z: uniform random floats in [-16.0, +16.0]
  *
  * @return sensor_sample_t  Populated sensor sample.
  */
 sensor_sample_t get_mock_sensor_data(void);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif  /* MOCK_SENSOR_H */
 