cmake_minimum_required(VERSION 3.13)
project(sensor_data_logger C)

# Enforce C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Public include path
include_directories(${PROJECT_SOURCE_DIR}/include)

# Fetch Unity framework
include(FetchContent)
FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG v2.5.2
)
FetchContent_MakeAvailable(unity)

# Enable CTest
enable_testing()

# Module source lists
set(SENSOR_LOGGER_SRC
    src/sensor_logger.c
)
set(MOCK_SENSOR_SRC
    src/mock_sensor.c
)

# Sensor Logger tests
add_executable(sensor_logger_tests
    ${SENSOR_LOGGER_SRC}
    tests/test_sensor_logger.c
)
target_link_libraries(sensor_logger_tests PRIVATE unity)
add_test(NAME sensor_logger_tests COMMAND sensor_logger_tests)

# Mock Sensor tests
add_executable(mock_sensor_tests
    ${SENSOR_LOGGER_SRC}   # for sensor_sample_t definition
    ${MOCK_SENSOR_SRC}
    tests/test_mock_sensor.c
)
target_link_libraries(mock_sensor_tests PRIVATE unity)
add_test(NAME mock_sensor_tests COMMAND mock_sensor_tests)

# Helper targets
add_custom_target(run-tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS sensor_logger_tests mock_sensor_tests
    COMMENT "Run all unit tests"
)

add_custom_target(static-analysis
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/static_analysis.sh
    COMMENT "Run MISRA-style static analysis"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMENT "Clean all build artifacts"
)
