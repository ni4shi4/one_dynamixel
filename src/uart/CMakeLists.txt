add_library(
  uart
  STATIC communication.c
)
target_include_directories(
  uart
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(
  uart
  pico_base
  util
)
