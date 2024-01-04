add_library(
  util
  crc.c analyze_packet.c packet_byte.c
)

target_include_directories(
  util
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(
  util
  pico_base
)
