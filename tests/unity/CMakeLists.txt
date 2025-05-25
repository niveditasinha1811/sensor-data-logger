cmake_minimum_required(VERSION 3.13)
project(UnityIntegration C)

include(FetchContent)

# Fetch Unity framework from official repository
FetchContent_Declare(
  unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG v2.5.2
)
FetchContent_MakeAvailable(unity)

# Create a Unity library target
add_library(unity STATIC
    ${unity_SOURCE_DIR}/src/unity.c
)

target_include_directories(unity PUBLIC
    ${unity_SOURCE_DIR}/src
)
