add_subdirectory(mock)

add_executable(
  test_dynamixel_app
  test_all.cpp
  test_communicate_device.cpp
  test_dynamixel_initialization.cpp
  test_dynamixel.cpp
)
target_link_libraries(
  test_dynamixel_app
  PRIVATE
    CppUTest
    CppUTestExt
    dynamixel_impl_interface
    mock_dynamixel
)
add_test(
  NAME test_dynamixel
  COMMAND $<TARGET_FILE:test_dynamixel_app>
)
