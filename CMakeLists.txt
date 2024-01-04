# Generated Cmake Pico project file
cmake_minimum_required(VERSION 3.25)

option(TEST_BUILD "Whether build with test config" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()


project(06_one_dynamixel)
enable_language(C CXX ASM)

# Initialise the Raspberry Pi Pico SDK (must be before add_subdirectory(src) and after project)
pico_sdk_init()

add_subdirectory(src)

add_subdirectory(examples)

if(TEST_BUILD)
  include(FetchContent)
  include(CTest)
  enable_testing()

  # FetchContent_Declare(
  #   unity
  #   GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  #   GIT_TAG v2.5.2
  # )
  # # unity_fixtureをオンにしている
  # FetchContent_MakeAvailable(unity)

  FetchContent_Declare(
    CppUTest
    GIT_REPOSITORY https://github.com/cpputest/cpputest.git
    GIT_TAG v4.0
  )
  set(TESTS OFF CACHE BOOL "Switch off CppUTest Test build")
  FetchContent_MakeAvailable(CppUTest)

  add_subdirectory(test)
endif()
