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
target_compile_definitions(sensor_logger_tests PRIVATE UNITY_TESTING)
target_link_libraries(sensor_logger_tests PRIVATE unity)

# Mock Sensor tests
add_executable(mock_sensor_tests
    ${SENSOR_LOGGER_SRC}   # for sensor_sample_t definition
    ${MOCK_SENSOR_SRC}
    tests/test_mock_sensor.c
)
# target_compile_definitions(mock_sensor_tests PRIVATE UNITY_TESTING) # Uncomment if mock_sensor.c uses UNITY_TESTING guards
target_link_libraries(mock_sensor_tests PRIVATE unity)

# Helper targets
add_custom_target(run-tests
    COMMAND $<TARGET_FILE:sensor_logger_tests>
    COMMAND $<TARGET_FILE:mock_sensor_tests>
    DEPENDS sensor_logger_tests mock_sensor_tests
    COMMENT "Run all Unity test executables"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(static-analysis
    COMMAND ${PROJECT_SOURCE_DIR}/scripts/static_analysis.sh
    COMMENT "Run MISRA-style static analysis"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMENT "Clean all build artifacts"
)