add_executable(
  one_dynamixel
  one_dynamixel.c
)

pico_set_program_name(one_dynamixel "one_dynamixel")
pico_set_program_version(one_dynamixel "0.1")

pico_enable_stdio_uart(one_dynamixel 0)
pico_enable_stdio_usb(one_dynamixel 1)

# Add the standard library to the build
target_link_libraries(
  one_dynamixel
  pico_stdlib
  hardware_uart
  uart
)

pico_add_extra_outputs(one_dynamixel)
