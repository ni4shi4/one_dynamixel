add_executable(
  test_util_app
  test_all.cpp
  test_crc.cpp
  test_analyze_packet.cpp
  test_packet_byte.cpp
)
target_link_libraries(
  test_util_app
  PRIVATE
    CppUTest
    util
)
add_test(
  NAME test_util
  COMMAND $<TARGET_FILE:test_util_app>
)
