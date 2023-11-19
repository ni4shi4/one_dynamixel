add_library(
  test_util
  test_crc.c
  test_crc_runner.c
  test_byte.c
  test_byte_runner.c
)
target_link_libraries(
  test_util
  PUBLIC unity
  PRIVATE util
)

add_executable(
  test_util_app
  test_util.c
)
target_link_libraries(
  test_util_app
  test_util
)

add_test(
  NAME test_util
  COMMAND $<TARGET_FILE:test_util_app>
)
