/**
 * @file    include/sensor_logger/sensor_logger.h
 * @brief   Public API for the Sensor Data Logger.
 *
 * @details
 *   - Stores timestamped accelerometer samples in a 128‐entry circular buffer.
 *   - Overwrites oldest on overflow.
 *   - Provides init, log, and print functions.
 *
 * @author  Nivedita
 * @date    2025-05-24
 */

 #ifndef SENSOR_LOGGER_H
 #define SENSOR_LOGGER_H
 
 #include <stdint.h>
 
 /** @brief Number of entries in the circular buffer */
 #define SENSOR_LOGGER_BUFFER_SIZE   (128U)
 
 /**
  * @brief   One timestamped accelerometer sample.
  */
 typedef struct
 {
     uint32_t timestamp_ms;  /**< Epoch timestamp in ms */
     float    acc_x;         /**< X-axis accel (G) */
     float    acc_y;         /**< Y-axis accel (G) */
     float    acc_z;         /**< Z-axis accel (G) */
 } sensor_sample_t;
 
 /**
  * @brief   Initialize/reset the logger.
  * @return  0 on success.
  */
 int init_sensor_logger(void);
 
 /**
  * @brief   Log one sample into the buffer.
  * @param[in] sample  Pointer to sample (must not be NULL).
  * @return  0 on success; -1 on NULL pointer.
  */
 int log_sensor_data(const sensor_sample_t * const p_sample_ptr);
 
 /**
  * @brief   Print stored samples (oldest→newest) as CSV.
  * @return  Number of characters printed.
  */
 int print_log(void);
 
 #ifdef UNITY_TESTING
 /**
  * @brief   (Test Only) Returns current number of stored entries.
  * @return  Entry count (0…BUFFER_SIZE).
  */
 uint32_t get_sensor_logger_entry_count(void);
 
 /**
  * @brief   (Test Only) Returns next write index.
  * @return  Head index (0…BUFFER_SIZE–1).
  */
 uint32_t get_sensor_logger_head_index(void);
 
 /**
  * @brief   (Test Only) Access raw buffer entry pointer.
  * @param[in] index  Buffer index.
  * @return  Pointer or NULL if out of range.
  */
 const sensor_sample_t* get_sensor_logger_buffer_entry(uint32_t index);
 #endif  /* UNITY_TESTING */
 
 #endif  /* SENSOR_LOGGER_H */
 