// main.c

#include <stdlib.h>
#include <time.h>
#include "sensor_logger/sensor_logger.h"
#include "mock_sensor/mock_sensor.h"

int main(void)
{
    // 1) Initialize/reset the circular logger
    init_sensor_logger();

    // 2) Seed the mock sensor RNG
    srand((unsigned)time(NULL));

    // 3) Generate & log N samples
    const int N = 200;
    for (int i = 0; i < N; ++i) {
        sensor_sample_t sample = get_mock_sensor_data();
        if (log_sensor_data(&sample) != 0) {
            // handle error (shouldn't happen with a valid pointer)
        }
    }

    // 4) Print all buffered samples to stdout as CSV
    //    Format per line: timestamp_ms,acc_x,acc_y,acc_z
    print_log();

    return 0;
}
