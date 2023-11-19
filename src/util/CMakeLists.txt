add_library(
  util
  STATIC crc.c byte.c
)

target_include_directories(
  util
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(
  util
  pico_base
)
