add_subdirectory(util)
add_subdirectory(dynamixel)
add_subdirectory(pico_communicator)

# テスト以外ではpicoをビルドする
if(NOT TEST_BUILD)
  add_subdirectory(pico)
endif()
