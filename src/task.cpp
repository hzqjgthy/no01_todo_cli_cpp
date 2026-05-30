#include "todo/task.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace todo {

Task TaskList::add(const std::string& title) {
    Task t;
    t.id         = next_id_++;
    t.title      = title;
    t.done       = false;
    t.created_at = std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count();
    tasks_.push_back(t);
    return t;
}

// ⚠️ 故意 bug：把 id 当成下标使用
// 修复方案见 README §五：改为按 Task::id 线性查找
bool TaskList::mark_done(int id) {
    // BUG: 应该按 Task::id 查找，这里直接当下标
    if (id < 0 || static_cast<size_t>(id) >= tasks_.size()) {
        return false;
    }
    tasks_[id].done = true;
    return true;
}

bool TaskList::remove(int id) {
    for (auto it = tasks_.begin(); it != tasks_.end(); ++it) {
        if (it->id == id) {
            tasks_.erase(it);
            return true;
        }
    }
    return false;
}

int TaskList::clear_done() {
    int removed = 0;
    auto new_end = std::remove_if(tasks_.begin(), tasks_.end(), [&](const Task& t) {
        if (t.done) {
            ++removed;
            return true;
        }
        return false;
    });
    tasks_.erase(new_end, tasks_.end());
    return removed;
}

void TaskList::set_state(int next_id, std::vector<Task> tasks) {
    next_id_ = next_id <= 0 ? 1 : next_id;
    tasks_   = std::move(tasks);
}

std::string format_time(std::int64_t unix_seconds) {
    std::time_t t = static_cast<std::time_t>(unix_seconds);
    std::tm     tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M");
    return oss.str();
}

}  // namespace todo
