/**
 * @file    src/main.c
 * @brief   Application entry point for the Sensor Data Logger demo.
 *          Initializes the logger, generates mock sensor samples, and emits
 *          the collected data as CSV to stdout.
 *
 * @details
 *   - Initializes a 128-entry circular buffer via init_sensor_logger()
 *   - Seeds the RNG and generates MAIN_NUM_SAMPLES_U32 mock samples
 *     (timestamp + three‐axis acceleration)
 *   - Logs each sample with log_sensor_data()
 *   - Finally dumps all stored samples via print_log()
 *
 * @author  Nivedita
 * @date    2025-05-26
 */

 #include <stdlib.h>     /* srand, RAND_MAX    */
 #include <time.h>       /* time, time_t       */
 
 #include "sensor_logger/sensor_logger.h"
 #include "mock_sensor/mock_sensor.h"
 
 /** @brief Number of mock samples to generate */
 #define MAIN_NUM_SAMPLES_U32  (200U)
 
 /**
  * @brief  Application entry point
  *
  * This function performs the following steps:
  *   1. Initialize/reset the circular sensor logger
  *   2. Seed the pseudo‐random generator for mock data
  *   3. Generate and log MAIN_NUM_SAMPLES_U32 samples
  *   4. Print all buffered samples as CSV via print_log()
  *
  * @return int  Always returns 0 (success)
  */
 int main(void)
 {
     /* Local variable declarations (all initialized) */
     uint32_t l_sample_count_U32 = MAIN_NUM_SAMPLES_U32;
     uint32_t l_loop_index_U32   = 0U;
     sensor_sample_t l_sample_st;
    
     /* 1. Initialize/reset the circular logger */
     (void)init_sensor_logger();
 
     /* 2. Seed the mock sensor RNG */
     srand((unsigned)time((time_t *)NULL));
 
     /* 3. Generate & log mock samples */
     for (l_loop_index_U32 = 0U;
          l_loop_index_U32 < l_sample_count_U32;
          ++l_loop_index_U32)
     {
         l_sample_st = get_mock_sensor_data();
 
         /* Expect success; ignore return-value per spec */
         (void)log_sensor_data(&l_sample_st);
     }
 
     /* 4. Dump CSV to stdout: timestamp_ms,acc_x,acc_y,acc_z */
     (void)print_log();
 
     return 0;
 }
 