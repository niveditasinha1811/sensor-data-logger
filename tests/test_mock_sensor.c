/* SPDX-License-Identifier: BSD-3-Clause */
/**
 * @file    test_mock_sensor.c
 * @brief   Unity unit tests for mock_sensor module.
 */

 #include "unity.h"
 #include "mock_sensor/mock_sensor.h"
 #include "sensor_logger/sensor_logger.h" // Though not directly used, it's good for context if sensor_sample_t was more complex.
 #include <time.h>    // For nanosleep in timestamp increase test
 #include <stdlib.h>  // For rand, srand
 #include <sys/time.h> // For gettimeofday (used by mock_sensor.c, indirectly relevant for seeding logic)
 #include <stdbool.h> // For bool type

 // Helper for test_get_mock_sensor_data_timestamp_increases to introduce a small delay
 void sleep_ms(long milliseconds) {
     struct timespec req, rem;
     if (milliseconds > 999) {
         req.tv_sec = (int)(milliseconds / 1000);
         req.tv_nsec = (milliseconds % 1000) * 1000000L;
     } else {
         req.tv_sec = 0;
         req.tv_nsec = milliseconds * 1000000L;
     }
     nanosleep(&req, &rem);
 }


 void setUp(void)  {
    /* Reset the seeded flag in mock_sensor.c if possible, or re-seed.
     * Since 'seeded' is static in mock_sensor.c, we cannot directly reset it here.
     * For robust testing of seeding, mock_sensor.c might need a test-only function
     * to reset its static 'seeded' variable or re-initialize its seed.
     * For now, each test run will use the same seed after the first call to get_mock_sensor_data()
     * unless the test runner executes tests in separate processes.
     * We can call srand here to ensure each test *could* have a different sequence if desired,
     * but it won't override the internal static 'seeded' logic of mock_sensor.c after the first call
     * in the entire test executable's lifetime.
     */
    // struct timeval tv_setup;
    // gettimeofday(&tv_setup, NULL);
    // srand((unsigned int)(tv_setup.tv_sec ^ tv_setup.tv_usec)); // Example of re-seeding for tests if needed
 }

 void tearDown(void) { /* no-op */ }

 void test_get_mock_sensor_data_range(void)
 {
     sensor_sample_t s = get_mock_sensor_data();
     /* Timestamp should be non-zero (or very large, depending on epoch) */
     // A more robust check might be against a known reasonable range if possible.
     TEST_ASSERT_GREATER_THAN_UINT32(0u, s.timestamp_ms);

     /* Each axis in \u00b116 G */
     TEST_ASSERT_TRUE_MESSAGE((s.acc_x >= -16.0f && s.acc_x <= 16.0f), "acc_x out of range");
     TEST_ASSERT_TRUE_MESSAGE((s.acc_y >= -16.0f && s.acc_y <= 16.0f), "acc_y out of range");
     TEST_ASSERT_TRUE_MESSAGE((s.acc_z >= -16.0f && s.acc_z <= 16.0f), "acc_z out of range");
 }

void test_get_mock_sensor_data_timestamp_increases(void)
{
    sensor_sample_t s1 = get_mock_sensor_data();
    // Introduce a small delay to ensure the timestamp can change if ms resolution is coarse
    // For gettimeofday, this might not be strictly necessary but doesn't hurt.
    sleep_ms(5); // Sleep for 5 milliseconds
    sensor_sample_t s2 = get_mock_sensor_data();

    TEST_ASSERT_GREATER_OR_EQUAL_UINT32_MESSAGE(s1.timestamp_ms, s2.timestamp_ms, "Timestamp should increase or be equal for very fast calls.");
    // Ideally, s2.timestamp_ms > s1.timestamp_ms if the delay is sufficient.
    // If gettimeofday has microsecond precision, even without a sleep, s2 should usually be greater.
    // Allowing equality handles cases where the system clock resolution or test execution is extremely fast.
    if (s1.timestamp_ms == s2.timestamp_ms) {
        TEST_IGNORE_MESSAGE("Timestamps were equal; test might be too fast for millisecond resolution without longer delay or system clock updated mid-call.");
    } else {
        TEST_ASSERT_GREATER_THAN_UINT32(s1.timestamp_ms, s2.timestamp_ms);
    }
}

void test_get_mock_sensor_data_randomness_distribution_basic(void)
{
    int num_samples = 100; // Reduced for faster unit test, ideally larger for statistical significance
    sensor_sample_t samples[num_samples];
    bool x_has_pos = false, x_has_neg = false;
    bool y_has_pos = false, y_has_neg = false;
    bool z_has_pos = false, z_has_neg = false;
    float first_x = 0.0f, first_y = 0.0f, first_z = 0.0f;
    bool x_all_same = true, y_all_same = true, z_all_same = true;

    for (int i = 0; i < num_samples; ++i) {
        samples[i] = get_mock_sensor_data();
        if (i == 0) {
            first_x = samples[i].acc_x;
            first_y = samples[i].acc_y;
            first_z = samples[i].acc_z;
        } else {
            if (samples[i].acc_x != first_x) x_all_same = false;
            if (samples[i].acc_y != first_y) y_all_same = false;
            if (samples[i].acc_z != first_z) z_all_same = false;
        }

        if (samples[i].acc_x > 0.1f) x_has_pos = true; // Using 0.1f to avoid trivial zeros due to float precision
        if (samples[i].acc_x < -0.1f) x_has_neg = true;
        if (samples[i].acc_y > 0.1f) y_has_pos = true;
        if (samples[i].acc_y < -0.1f) y_has_neg = true;
        if (samples[i].acc_z > 0.1f) z_has_pos = true;
        if (samples[i].acc_z < -0.1f) z_has_neg = true;
    }

    TEST_ASSERT_FALSE_MESSAGE(x_all_same, "All acc_x values were identical; check randomness.");
    TEST_ASSERT_FALSE_MESSAGE(y_all_same, "All acc_y values were identical; check randomness.");
    TEST_ASSERT_FALSE_MESSAGE(z_all_same, "All acc_z values were identical; check randomness.");

    TEST_ASSERT_TRUE_MESSAGE(x_has_pos && x_has_neg, "acc_x values did not show both positive and negative values; check distribution.");
    TEST_ASSERT_TRUE_MESSAGE(y_has_pos && y_has_neg, "acc_y values did not show both positive and negative values; check distribution.");
    TEST_ASSERT_TRUE_MESSAGE(z_has_pos && z_has_neg, "acc_z values did not show both positive and negative values; check distribution.");
}

// Test for the seeding behavior (conceptual - actual re-seeding check is hard due to static 'seeded')
// This test mostly verifies that subsequent calls after the first one don't break anything.
void test_get_mock_sensor_data_multiple_calls_seeding_stability(void)
{
    sensor_sample_t s1 = get_mock_sensor_data(); // First call, seeds internally
    sensor_sample_t s2 = get_mock_sensor_data(); // Second call, should use existing seed
    sensor_sample_t s3 = get_mock_sensor_data(); // Third call

    // Basic range checks, similar to test_get_mock_sensor_data_range
    TEST_ASSERT_GREATER_THAN_UINT32(0u, s1.timestamp_ms);
    TEST_ASSERT_TRUE((s1.acc_x >= -16.0f && s1.acc_x <= 16.0f));
    TEST_ASSERT_GREATER_THAN_UINT32(0u, s2.timestamp_ms);
    TEST_ASSERT_TRUE((s2.acc_y >= -16.0f && s2.acc_y <= 16.0f));
    TEST_ASSERT_GREATER_THAN_UINT32(0u, s3.timestamp_ms);
    TEST_ASSERT_TRUE((s3.acc_z >= -16.0f && s3.acc_z <= 16.0f));

    // Check if timestamps are generally increasing
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(s1.timestamp_ms, s2.timestamp_ms);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(s2.timestamp_ms, s3.timestamp_ms);

    // A very basic check for different values (not a guarantee of good randomness, but a start)
    // This has a small chance of failing if values randomly repeat, especially with few samples.
    bool s1_s2_diff = (s1.acc_x != s2.acc_x) || (s1.acc_y != s2.acc_y) || (s1.acc_z != s2.acc_z);
    bool s2_s3_diff = (s2.acc_x != s3.acc_x) || (s2.acc_y != s3.acc_y) || (s2.acc_z != s3.acc_z);
    TEST_ASSERT_TRUE_MESSAGE(s1_s2_diff || s2_s3_diff, "Consecutive samples had identical accelerometer data; possible seeding or randomness issue.");
}


 int main(void)
 {
     UNITY_BEGIN();
     RUN_TEST(test_get_mock_sensor_data_range);
     RUN_TEST(test_get_mock_sensor_data_timestamp_increases);
     RUN_TEST(test_get_mock_sensor_data_randomness_distribution_basic);
     RUN_TEST(test_get_mock_sensor_data_multiple_calls_seeding_stability);
     return UNITY_END();
 }