// task.hpp - 待办事项的领域模型与集合操作
//
// 设计原则：
//   - Task 是纯数据，不依赖任何 IO；
//   - TaskList 提供增删改查，便于单测；
//   - 持久化交给 store.hpp，本头文件零文件 IO。
#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace todo {

struct Task {
    int         id        = 0;
    std::string title;
    bool        done      = false;
    // 用 system_clock 的 time_t（秒级 unix 时间戳）方便 JSON 序列化
    std::int64_t created_at = 0;
};

class TaskList {
public:
    TaskList() = default;

    // 增：返回新任务（含分配好的 ID）
    Task add(const std::string& title);

    // 改：按 ID 标记完成。
    // ⚠️ 当前实现存在故意 bug：把 id 当下标用。
    // 这是 README 里规划的"故意 bug"，将在后续提交里用 GDB 调试后修复。
    bool mark_done(int id);

    // 删：按 ID 删除，返回是否删掉了
    bool remove(int id);

    // 清：清掉所有已完成的，返回清掉的条数
    int clear_done();

    // 只读访问
    const std::vector<Task>& tasks() const noexcept { return tasks_; }
    int                      next_id() const noexcept { return next_id_; }

    // 反序列化时由 store 直接灌入
    void set_state(int next_id, std::vector<Task> tasks);

private:
    int               next_id_ = 1;
    std::vector<Task> tasks_;
};

// 把 unix 时间戳格式化成 "2006-01-02 15:04"
std::string format_time(std::int64_t unix_seconds);

}  // namespace todo
