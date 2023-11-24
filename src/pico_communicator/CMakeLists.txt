# Define header-only library
add_library(
  pico_communicator_headers INTERFACE
)
target_include_directories(
  pico_communicator_headers
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(
  pico_communicator_headers
  INTERFACE
    pico_base
    hardware_uart_headers
)

add_library(
  pico_communicator
  pico_communicator.c
)
if(PICO_PLATFORM STREQUAL "host")
  target_sources(
    pico_communicator
    PRIVATE pico_communicator_extra_host.c
  )
else()
  target_sources(
    pico_communicator
    PRIVATE pico_communicator_extra.c
  )
endif()
target_link_libraries(
  pico_communicator
  PRIVATE
    pico_communicator_headers
    hardware_uart
)
