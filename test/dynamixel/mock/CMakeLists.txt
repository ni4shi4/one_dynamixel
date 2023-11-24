# define mock library for test
add_library(
  mock_dynamixel
  mock_hardware_gpio.cpp
  mock_pico_communicator.cpp
)
target_link_libraries(
  mock_dynamixel
  PRIVATE
    pico_base
    hardware_gpio_headers
    pico_communicator_headers
    CppUTest
    CppUTestExt
)
