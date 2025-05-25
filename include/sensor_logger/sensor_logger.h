/* SPDX-License-Identifier: BSD-3-Clause */
/**
 * @file    sensor_logger.h
 * @brief   Public API for sensor data logging (circular buffer, timestamped).
 *
 * Implements a circular buffer of SENSOR_LOGGER_BUFFER_SIZE samples,
 * each containing a 32-bit ms-timestamp and three float accelerations (\u00b116 G).
 */

 #ifndef SENSOR_LOGGER_H
 #define SENSOR_LOGGER_H
 
 #include <stdint.h>
 
 /** @brief Number of entries in the circular buffer */
 #define SENSOR_LOGGER_BUFFER_SIZE   (128U)
 
 /**
  * @brief   Single sensor sample: timestamp + X/Y/Z accelerations.
  */
 typedef struct
 {
     uint32_t timestamp_ms;    /**< Epoch timestamp (ms resolution) */
     float    acc_x;           /**< X-axis acceleration (G) */
     float    acc_y;           /**< Y-axis acceleration (G) */
     float    acc_z;           /**< Z-axis acceleration (G) */
 } sensor_sample_t;
 
 /**
  * @brief   Initialize the sensor logger module.
  *
  * Must be called before any other logger API. Clears the buffer.
  *
  * @return  0 on success; non-zero error code otherwise.
  */
 int init_sensor_logger(void);
 
 /**
  * @brief   Log a new sample into the circular buffer.
  *
  * Overwrites the oldest entry when the buffer is full.
  *
  * @param[in]  sample  Pointer to the sample to store.
  * @return     0 on success; non-zero error code otherwise.
  */
 int log_sensor_data(const sensor_sample_t * sample);
 
 /**
  * @brief   Print all logged samples to console (CSV format).
  *
  * Uses LOG_PRINTF macro, producing lines of:
  * timestamp_ms,acc_x,acc_y,acc_z\n
  *
  * @return  Number of characters printed.
  */
 int print_log(void);
 
 #ifdef UNITY_TESTING
 /**
  * @brief   (Test Only) Get the current number of valid entries in the buffer.
  * @return  Number of entries.
  */
 uint32_t get_sensor_logger_entry_count(void);
 
 /**
  * @brief   (Test Only) Get the current head index (next write position).
  * @return  Head index.
  */
 uint32_t get_sensor_logger_head_index(void);
 
 /**
  * @brief   (Test Only) Get a direct pointer to a buffer entry.
  * @param[in] index Index of the buffer entry to retrieve.
  * @return  Pointer to the sensor_sample_t at the given index, or NULL if index is out of bounds.
  */
 const sensor_sample_t* get_sensor_logger_buffer_entry(uint32_t index);
 #endif /* UNITY_TESTING */
 
 #endif /* SENSOR_LOGGER_H */