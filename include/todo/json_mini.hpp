// json_mini.hpp - 极简 JSON 序列化/反序列化（仅服务于 TaskList）
//
// 为什么不引入 nlohmann/json？
//   1. 学习项目，体现"不用三方库也能搞定";
//   2. TaskList 的 JSON 结构非常固定（4 字段），手写 < 100 行;
//   3. 便于 GDB 单步看清解析过程。
//
// 真要扩展功能（嵌套对象、数组、Unicode 转义等）请直接换 nlohmann/json，
// 当前实现只覆盖本项目用到的 JSON 子集。
#pragma once

#include <stdexcept>
#include <string>

#include "todo/task.hpp"

namespace todo::json_mini {

class ParseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

// 把 TaskList 序列化为 JSON 文本（带 2 空格缩进）
std::string dump(const TaskList& list);

// 反序列化：输入 JSON 文本，回填到 list
void parse(const std::string& text, TaskList& list);

}  // namespace todo::json_mini
