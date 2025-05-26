/**
 * @file    src/mock_sensor.c
 * @brief   Mock sensor data generator implementation.
 *
 * @details
 *   - On first call, seeds the PRNG from the current time (ms resolution)
 *   - Each call returns a sensor_sample_t with:
 *       * timestamp_ms: epoch milliseconds
 *       * acc_x, acc_y, acc_z: uniform random floats in [-16.0, +16.0]
 *
 * @author  Nivedita
 * @date    2025-05-24
 */

 #include "mock_sensor/mock_sensor.h"
 #include <stdlib.h>       /* srand, RAND_MAX */
 #include <sys/time.h>     /* gettimeofday, struct timeval */
 #include <stdbool.h>      /* bool */
 
 /** @brief Milliseconds per second */
 #define MOCK_SENSOR_MS_PER_SEC_U32   (1000UL)
 
 /*— Global flag indicating if RNG has been seeded —*/
 static bool _bool_seeded_b = false;
 
 /**
  * @brief  Generate and return one mock accelerometer sample.
  *
  * On the first invocation, seeds the RNG with a mix of seconds and microseconds.
  * Subsequent calls reuse the generator state.
  *
  * @return sensor_sample_t  Fully populated sample.
  */
 sensor_sample_t get_mock_sensor_data(void)
 {
     struct timeval l_mock_sensor_tv0_st = { 0 };
     struct timeval l_mock_sensor_tv_st  = { 0 };
     sensor_sample_t l_sample_st = {
                                    .timestamp_ms = 0U,
                                    .acc_x        = 0.0f,
                                    .acc_y        = 0.0f,
                                    .acc_z        = 0.0f
                                    };
    

     /* 1) Seed RNG once */
     if (_bool_seeded_b == false)
     {
         (void)gettimeofday(&l_mock_sensor_tv0_st, NULL);
         /* XOR seconds and microseconds for variability */
         srand((unsigned int)
               ( (unsigned long)l_mock_sensor_tv0_st.tv_sec ^
                 (unsigned long)l_mock_sensor_tv0_st.tv_usec ));
         _bool_seeded_b = true;
     }
 
     /* 2) Timestamp in milliseconds */
     (void)gettimeofday(&l_mock_sensor_tv_st, NULL);
     l_sample_st.timestamp_ms =
         (uint32_t)
         ( ((uint32_t)l_mock_sensor_tv_st.tv_sec * MOCK_SENSOR_MS_PER_SEC_U32) +
           ((uint32_t)l_mock_sensor_tv_st.tv_usec / MOCK_SENSOR_MS_PER_SEC_U32) );
 
     /* 3) Generate uniform random acceleration in [-16.0, +16.0] */
     for (uint32_t l_axis_index_U32 = 0U;
          l_axis_index_U32 < 3U;
          ++l_axis_index_U32)
     {
         float l_normalized_f = (float)rand() / (float)RAND_MAX;  /* [0.0..1.0] */
         float *ptr_axis_acc_f = (l_axis_index_U32 == 0U) ? &l_sample_st.acc_x
                                 : (l_axis_index_U32 == 1U) ? &l_sample_st.acc_y
                                                           : &l_sample_st.acc_z;
 
         /* Map to [-16.0..+16.0] */
         *ptr_axis_acc_f = (l_normalized_f * 32.0f) - 16.0f;
     }
 
     return l_sample_st;
 }
 