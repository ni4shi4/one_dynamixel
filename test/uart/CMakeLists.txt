add_executable(
  test_communication_app
  test_communication.c
)

target_link_libraries(
  test_communication_app
  unity
  uart
)

add_test(
  NAME test_communication
  COMMAND $<TARGET_FILE:test_communication_app>
)
