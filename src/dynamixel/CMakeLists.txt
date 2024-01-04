# Define first header-only library
add_library(
  dynamixel_headers INTERFACE
)
target_include_directories(
  dynamixel_headers
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(
  dynamixel_headers
  INTERFACE
    pico_base
    pico_time
    pico_communicator_headers
    hardware_gpio_headers
    util
)

# define interface with source file (to separate from header only libirary)
# add_library(... INTERFACE source.cだとうまく行かなかった)
add_library(
  dynamixel_impl_interface INTERFACE
)
target_sources(
  dynamixel_impl_interface
  PUBLIC dynamixel.c
)
target_link_libraries(
  dynamixel_impl_interface
  INTERFACE dynamixel_headers
)

# define library for real device
add_library(
  dynamixel
)
target_link_libraries(
  dynamixel
  PUBLIC
    dynamixel_impl_interface
    hardware_gpio
    m
  PRIVATE
    pico_communicator
)
