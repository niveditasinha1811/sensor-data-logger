/* SPDX-License-Identifier: BSD-3-Clause */
/**
 * @file    mock_sensor.c
 * @brief   Implementation of mock sensor data generator.
 *
 * Uses gettimeofday() for epoch-ms timestamp and rand()
 * for uniform random ±16 G on each axis.
 */

 #include "mock_sensor/mock_sensor.h"
 #include <stdlib.h>
 #include <sys/time.h>
 
 /**
  * @brief Generate and return a mock sensor sample.
  */
 sensor_sample_t get_mock_sensor_data(void)
 {
     static int seeded = 0;
     if (!seeded)
     {
         struct timeval tv0;
         (void)gettimeofday(&tv0, NULL);
         srand((unsigned int)(tv0.tv_sec ^ tv0.tv_usec));
         seeded = 1;
     }
 
     sensor_sample_t sample;
 
     /* Timestamp: epoch ms */
     struct timeval tv;
     (void)gettimeofday(&tv, NULL);
     sample.timestamp_ms = (uint32_t)((tv.tv_sec * 1000UL) + (tv.tv_usec / 1000UL));
 
     /* Generate uniform random floats in [-16.0, +16.0] */
     for (int i = 0; i < 3; ++i)
     {
         float normalized = (float)rand() / (float)RAND_MAX;  /* [0..1] */
         float *acc = (i == 0) ? &sample.acc_x
                       : (i == 1) ? &sample.acc_y
                                  : &sample.acc_z;
         *acc = (normalized * 32.0f) - 16.0f;
     }
 
     return sample;
 }
 