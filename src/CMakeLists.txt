add_subdirectory(util)
add_subdirectory(spi)

# テスト以外ではpicoをビルドする
if(NOT TEST_BUILD)
  add_subdirectory(pico)
endif()
