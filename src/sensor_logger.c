/**
 * @file    src/sensor_logger.c
 * @brief   Implementation of the sensor data logger API.
 *
 * @details
 *   - Maintains a 128‐entry circular buffer of timestamped accelerometer samples.
 *   - Overwrites the oldest sample when full.
 *   - Exposes init, log, and print functions.
 *
 * @author  Nivedita
 * @date    2025-05-24
 */

 #include "sensor_logger/sensor_logger.h"
 #include <string.h>     /* memset */
 #include <stdio.h>      /* printf, fflush */
 #include <stdint.h>     /* uint32_t */
 
 /* If not provided, these can be overridden */
 #ifndef LOG_PRINTF
 #define LOG_PRINTF(...)    (printf(__VA_ARGS__))
 #endif
 #ifndef ENTER_CRITICAL
 #define ENTER_CRITICAL()   do { } while (0)
 #endif
 #ifndef EXIT_CRITICAL
 #define EXIT_CRITICAL()    do { } while (0)
 #endif
 
 /*— Global storage —*/
 static sensor_sample_t gl_sensor_logger_buffer_U8a[SENSOR_LOGGER_BUFFER_SIZE];
 static uint32_t        gl_sensor_logger_head_index_U32   = 0U;  /**< Next write position */
 static uint32_t        gl_sensor_logger_entry_count_U32  = 0U;  /**< Number of valid entries */
 
 /**
  * @brief   Initialize/reset the sensor logger.
  * @return  0 on success.
  */
 int init_sensor_logger(void)
 {
     ENTER_CRITICAL();
       gl_sensor_logger_head_index_U32  = 0U;
       gl_sensor_logger_entry_count_U32 = 0U;
       (void)memset(gl_sensor_logger_buffer_U8a, 0, sizeof(gl_sensor_logger_buffer_U8a));
     EXIT_CRITICAL();
     return 0;
 }
 
 /**
  * @brief   Log one accelerometer sample.
  * @param[in] p_sample_ptr  Pointer to sample (must not be NULL).
  * @return  0 on success; -1 if p_sample_ptr == NULL.
  */
 int log_sensor_data(const sensor_sample_t * const p_sample_ptr)
 {
     if (p_sample_ptr == NULL)
     {
         return -1;
     }
 
     ENTER_CRITICAL();
       /* Write and advance head index */
       gl_sensor_logger_buffer_U8a[gl_sensor_logger_head_index_U32] = *p_sample_ptr;
       gl_sensor_logger_head_index_U32 =
           (gl_sensor_logger_head_index_U32 + 1U) % SENSOR_LOGGER_BUFFER_SIZE;
 
       /* Increase count up to capacity */
       if (gl_sensor_logger_entry_count_U32 < SENSOR_LOGGER_BUFFER_SIZE)
       {
           gl_sensor_logger_entry_count_U32++;
       }
     EXIT_CRITICAL();
 
     return 0;
 }
 
 /**
  * @brief   Print all logged samples in CSV (oldest→newest).
  * @return  Number of characters printed.
  */
 int print_log(void)
 {
     uint32_t l_printed_chars_U32    = 0U;
     uint32_t l_current_index_U32    = 0U;
     uint32_t l_loop_index_U32       = 0U;
 
     ENTER_CRITICAL();
       if (gl_sensor_logger_entry_count_U32 == 0U)
       {
           EXIT_CRITICAL();
           return 0;
       }
 
       /* Determine start index */
       if (gl_sensor_logger_entry_count_U32 < SENSOR_LOGGER_BUFFER_SIZE)
       {
           l_current_index_U32 = 0U;
       }
       else
       {
           l_current_index_U32 = gl_sensor_logger_head_index_U32;
       }
 
       /* Iterate over valid entries */
       for (l_loop_index_U32 = 0U;
            l_loop_index_U32 < gl_sensor_logger_entry_count_U32;
            ++l_loop_index_U32)
       {
           uint32_t l_actual_buffer_index_U32 =
               (l_current_index_U32 + l_loop_index_U32) % SENSOR_LOGGER_BUFFER_SIZE;
           const sensor_sample_t * const ptr_buffer_entry_st =
               &gl_sensor_logger_buffer_U8a[l_actual_buffer_index_U32];
 
           int l_ret_i = LOG_PRINTF(
               "%lu,%.6f,%.6f,%.6f\n",
               (unsigned long)ptr_buffer_entry_st->timestamp_ms,
               ptr_buffer_entry_st->acc_x,
               ptr_buffer_entry_st->acc_y,
               ptr_buffer_entry_st->acc_z
           );
           if (l_ret_i > 0)
           {
               l_printed_chars_U32 += (uint32_t)l_ret_i;
           }
       }
     EXIT_CRITICAL();
 
     /* Ensure any redirected stdout (e.g. fmemopen) is flushed */
     (void)fflush(stdout);
 
     return (int)l_printed_chars_U32;
 }
 
 #ifdef UNITY_TESTING

 /* cppcheck-suppress unusedFunction */
 uint32_t get_sensor_logger_entry_count(void)
 {
     return gl_sensor_logger_entry_count_U32;
 }
 
 /* cppcheck-suppress unusedFunction */
 uint32_t get_sensor_logger_head_index(void)
 {
     return gl_sensor_logger_head_index_U32;
 }
 
 /* cppcheck-suppress unusedFunction */
 const sensor_sample_t* get_sensor_logger_buffer_entry(uint32_t index)
 {
     if (index < SENSOR_LOGGER_BUFFER_SIZE)
     {
         return &gl_sensor_logger_buffer_U8a[index];
     }
     return NULL;
 }
 
 #endif  /* UNITY_TESTING */
 