cmake_minimum_required(VERSION 3.16)
project(moonwake_perception_basics VERSION 0.1.0 LANGUAGES CXX)

# C++17 标准与编译器设定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # 导出编译命令，便于 VS Code / clangd / IDE 补全

# 核心感知业务逻辑库
add_library(moonwake_perception
  src/buoy_offset.cpp
)

target_include_directories(moonwake_perception
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 命令行测试可执行程序
add_executable(buoy_offset_demo
  src/buoy_offset_demo.cpp
)

target_link_libraries(buoy_offset_demo
  PRIVATE moonwake_perception
)

# 单元测试（使用 CTest）
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
  enable_testing()
  add_executable(test_buoy_offset
    tests/test_buoy_offset.cpp
  )
  target_link_libraries(test_buoy_offset
    PRIVATE moonwake_perception
  )
  add_test(NAME buoy_offset_unit_tests COMMAND test_buoy_offset)
endif()
