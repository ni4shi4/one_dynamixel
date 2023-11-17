add_library(
  spi
  STATIC communication.c
)
target_include_directories(
  spi
  PUBLIC ${PROJECT_SOURCE_DIR}/include/spi
)
target_link_libraries(
  spi
  pico_base
  util
)
