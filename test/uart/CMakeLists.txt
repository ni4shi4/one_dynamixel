# See https://github.com/ThrowTheSwitch/Unity/tree/master/examples/example_2 for more detail

add_library(
  test_uart
  test_communication_runner.c
  test_communication.c
)

target_link_libraries(
  test_uart
  PUBLIC unity
  PRIVATE uart
)

# uartのテスト
add_executable(
  test_uart_app
  test_uart.c
)

target_link_libraries(
  test_uart_app
  test_uart
)

add_test(
  NAME test_uart
  COMMAND $<TARGET_FILE:test_uart_app>
)
