add_library(
  util
  STATIC crc.c byte.c
)

target_include_directories(
  util
  PUBLIC ${PROJECT_SOURCE_DIR}/include/util
)

target_link_libraries(
  util
  pico_base
)