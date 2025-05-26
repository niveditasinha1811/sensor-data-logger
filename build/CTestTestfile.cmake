# CMake generated Testfile for 
# Source directory: /Users/niveditasinha/Downloads/sensor_data_logger
# Build directory: /Users/niveditasinha/Downloads/sensor_data_logger/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(sensor_logger_tests "/Users/niveditasinha/Downloads/sensor_data_logger/build/sensor_logger_tests")
set_tests_properties(sensor_logger_tests PROPERTIES  _BACKTRACE_TRIPLES "/Users/niveditasinha/Downloads/sensor_data_logger/CMakeLists.txt;37;add_test;/Users/niveditasinha/Downloads/sensor_data_logger/CMakeLists.txt;0;")
add_test(mock_sensor_tests "/Users/niveditasinha/Downloads/sensor_data_logger/build/mock_sensor_tests")
set_tests_properties(mock_sensor_tests PROPERTIES  _BACKTRACE_TRIPLES "/Users/niveditasinha/Downloads/sensor_data_logger/CMakeLists.txt;49;add_test;/Users/niveditasinha/Downloads/sensor_data_logger/CMakeLists.txt;0;")
subdirs("_deps/unity-build")
