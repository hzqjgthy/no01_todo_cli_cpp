# 极简自写测试：每个 test_*.cpp 都是独立可执行文件，main 函数返回非零即失败。
# 不引入 GoogleTest，是为了练 C++ 基本功；后续真要规模化测试再换 GTest。

function(add_simple_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE todo_core)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_simple_test(test_task)
add_simple_test(test_store)
