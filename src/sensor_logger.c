/* SPDX-License-Identifier: BSD-3-Clause */
/**
 * @file    sensor_logger.c
 * @brief   Implementation of the sensor data logger API.
 *
 * Implements a 128-entry circular buffer of timestamped accelerometer samples,
 * with overwrite-oldest behavior and simple critical-section guards.
 */

#include "sensor_logger/sensor_logger.h"
#include <string.h>     /* for memset */
#include <stdio.h>      /* for printf */
#include <stdint.h>     /* for uint32_t */

/* If not provided by the platform, you can override these */
#ifndef LOG_PRINTF
#define LOG_PRINTF(...)    (printf(__VA_ARGS__))
#endif
#ifndef ENTER_CRITICAL
#define ENTER_CRITICAL() do { } while (0)
#endif
#ifndef EXIT_CRITICAL
#define EXIT_CRITICAL()  do { } while (0)
#endif

/* Circular buffer storage */
static sensor_sample_t buffer[SENSOR_LOGGER_BUFFER_SIZE];
static uint32_t head_index = 0;   /**< Next write position */
static uint32_t entry_count = 0;  /**< Number of valid entries */

/**
 * @brief Initialize the sensor logger module.
 *
 * Clears buffer state; must be called before other API functions.
 *
 * @return 0 on success.
 */
int init_sensor_logger(void)
{
    ENTER_CRITICAL();
    head_index  = 0u;
    entry_count = 0u;
    /* zero out buffer for deterministic startup (optional) */
    (void)memset(buffer, 0, sizeof(buffer));
    EXIT_CRITICAL();
    return 0;
}

/**
 * @brief Log a new sample into the circular buffer.
 *
 * Overwrites the oldest entry when full.
 *
 * @param[in] sample Pointer to the sample to store (must not be NULL).
 * @return 0 on success; -1 if sample was NULL.
 */
int log_sensor_data(const sensor_sample_t * sample)
{
    if (sample == NULL)
    {
        return -1;
    }

    ENTER_CRITICAL();
    /* write at head_index */
    buffer[head_index] = *sample;

    /* advance head, wrap if needed */
    head_index = (head_index + 1u) % SENSOR_LOGGER_BUFFER_SIZE;

    /* track count up to capacity */
    if (entry_count < SENSOR_LOGGER_BUFFER_SIZE)
    {
        entry_count++;
    }
    EXIT_CRITICAL();

    return 0;
}

/**
 * @brief Print all logged samples in chronological order.
 *
 * Uses CSV format: timestamp_ms,acc_x,acc_y,acc_z\n
 * Returns total characters printed.
 *
 * @return total number of characters printed (sum of LOG_PRINTF returns).
 */
int print_log(void)
{
    uint32_t printed_chars = 0u;

    ENTER_CRITICAL();
    /* determine start of oldest entry */
    uint32_t start = (head_index + SENSOR_LOGGER_BUFFER_SIZE
                      - entry_count) % SENSOR_LOGGER_BUFFER_SIZE;

    /* iterate oldest → newest */
    for (uint32_t i = 0u; i < entry_count; ++i)
    {
        uint32_t idx = (start + i) % SENSOR_LOGGER_BUFFER_SIZE;
        sensor_sample_t *s = &buffer[idx];

        int ret = LOG_PRINTF("%lu,%.6f,%.6f,%.6f\n",
                             (unsigned long)s->timestamp_ms,
                             s->acc_x, s->acc_y, s->acc_z);
        if (ret > 0)
        {
            printed_chars += (uint32_t)ret;
        }
    }
    EXIT_CRITICAL();

    return (int)printed_chars;
}
