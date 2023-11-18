add_library(
  uart
  STATIC communication.c
)
target_include_directories(
  uart
  PUBLIC ${PROJECT_SOURCE_DIR}/include/uart
)
target_link_libraries(
  uart
  pico_base
  util
)
