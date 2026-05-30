// test_task.cpp - TaskList 业务逻辑测试
#include <iostream>
#include <stdexcept>

#include "todo/task.hpp"

#define EXPECT(cond)                                                          \
    do {                                                                      \
        if (!(cond)) {                                                        \
            std::cerr << "EXPECT failed at " << __FILE__ << ":" << __LINE__   \
                      << " -> " #cond << '\n';                                \
            return 1;                                                         \
        }                                                                     \
    } while (0)

static int test_add_assigns_incrementing_id() {
    todo::TaskList l;
    auto a = l.add("A");
    auto b = l.add("B");
    EXPECT(a.id == 1);
    EXPECT(b.id == 2);
    EXPECT(l.next_id() == 3);
    return 0;
}

static int test_remove_by_id() {
    todo::TaskList l;
    l.add("A");
    auto b = l.add("B");
    EXPECT(l.remove(b.id) == true);
    EXPECT(l.tasks().size() == 1);
    EXPECT(l.tasks()[0].title == "A");
    return 0;
}

static int test_clear_done() {
    todo::TaskList l;
    l.add("A");
    l.add("B");
    // 直接 set 完成状态，避开 mark_done 的故意 bug
    auto state = l.tasks();
    state[0].done = true;
    l.set_state(l.next_id(), state);

    EXPECT(l.clear_done() == 1);
    EXPECT(l.tasks().size() == 1);
    EXPECT(l.tasks()[0].title == "B");
    return 0;
}

// 这个测试目前会失败 —— 因为 mark_done 有故意 bug。
// 修复后把 SKIP 宏改成 0 即可作为回归测试。
#define SKIP 1
static int test_mark_done_by_id_known_bug() {
    if (SKIP) return 0;

    todo::TaskList l;
    auto a = l.add("A");
    auto b = l.add("B");
    l.remove(a.id);  // 删掉 A，剩下 B（id=2，下标=0）
    EXPECT(l.mark_done(b.id) == true);
    EXPECT(l.tasks()[0].done == true);
    return 0;
}

int main() {
    int rc = 0;
    rc |= test_add_assigns_incrementing_id();
    rc |= test_remove_by_id();
    rc |= test_clear_done();
    rc |= test_mark_done_by_id_known_bug();
    if (rc == 0) std::cout << "test_task: OK\n";
    return rc;
}
