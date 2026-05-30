// store.hpp - 把 TaskList 序列化到本地 JSON 文件
//
// 数据文件路径：
//   - 默认：~/.todo/data.json （Linux/macOS） 或 %USERPROFILE%\.todo\data.json （Windows）
//   - 也可通过环境变量 TODO_HOME 覆盖（便于测试）
#pragma once

#include <filesystem>
#include <string>

#include "todo/task.hpp"

namespace todo::store {

// 数据文件绝对路径
std::filesystem::path data_path();

// 读：文件不存在时返回空 TaskList（首次运行场景）
TaskList load();

// 写：先写临时文件再 rename，避免半截文件
void save(const TaskList& list);

}  // namespace todo::store
