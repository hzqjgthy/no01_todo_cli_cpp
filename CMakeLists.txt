cmake_minimum_required(VERSION 3.16)
project(no01_todo_cli_cpp
    VERSION 0.1.0
    DESCRIPTION "命令行待办工具（C++ 版）"
    LANGUAGES CXX)

# ---------- 全局编译选项 ----------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 默认构建类型：未指定时用 Debug（方便 GDB）
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# 输出 compile_commands.json 给 clangd / vscode 用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 编译告警：尽量严
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------- 核心库（业务 + 持久化） ----------
add_library(todo_core STATIC
    src/task.cpp
    src/store.cpp
    src/json_mini.cpp
)
target_include_directories(todo_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---------- 可执行文件 ----------
add_executable(todo src/main.cpp)
target_link_libraries(todo PRIVATE todo_core)

# ---------- 测试 ----------
enable_testing()
add_subdirectory(tests)

# ---------- 安装（可选） ----------
install(TARGETS todo RUNTIME DESTINATION bin)
