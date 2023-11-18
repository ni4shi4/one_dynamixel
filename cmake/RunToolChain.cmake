# presetでtoolchainfileを指定(=CMAKE_TOOLCHAIN_FILEを指定)すると、
# pico-sdkで読み込むように設定しているtoolchainfileが読み込まないようになってしまうので、
# コンパイル時のオプションが正しく設定できなくなる。
# このため、pico向けにbuildする際には、presetでtoolchainfileを指定しないようにする
# https://github.com/raspberrypi/pico-sdk/blob/master/cmake/pico_pre_load_toolchain.cmake
# https://github.com/raspberrypi/pico-sdk/blob/master/cmake/preload/toolchains/pico_arm_gcc.cmake

# presetで読み込む場合は、toolchainファイルを自分で読み込む
include($ENV{PICO_SDK_PATH}/cmake/preload/toolchains/pico_arm_gcc.cmake)

# もしくは自分で必要な設定を入力する
# set(CMAKE_C_COMPILER arm-none-eabi-gcc)
# set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
# set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m0plus -mthumb" CACHE STRING "Flags used by the C compiler during all build types.")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-m0plus -mthumb" CACHE STRING "Flags used by the CXX compiler during all build types.")
# set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m0plus -mthumb" CACHE STRING "Flags used by the ASM compiler during all build types.")

# # Without this flag, CMake is unable to pass the test compilation check
# # https://discourse.cmake.org/t/the-c-compiler-is-not-able-to-compile-a-simple-test-program-when-compiling-using-arm-gnu-toolchain/8215
# set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
