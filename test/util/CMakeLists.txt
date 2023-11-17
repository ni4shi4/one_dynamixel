add_executable(
  test_crc_app
  test_crc.c
)
add_executable(
  test_byte_app
  test_byte.c
)

target_link_libraries(
  test_crc_app
  unity util
)
target_link_libraries(
  test_byte_app
  unity util
)

add_test(
  NAME test_crc
  COMMAND $<TARGET_FILE:test_crc_app>
)
add_test(
  NAME test_byte
  COMMAND $<TARGET_FILE:test_byte_app>
)