// test_store.cpp - 持久化往返测试
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>

#include "todo/store.hpp"
#include "todo/task.hpp"

#define EXPECT(cond)                                                          \
    do {                                                                      \
        if (!(cond)) {                                                        \
            std::cerr << "EXPECT failed at " << __FILE__ << ":" << __LINE__   \
                      << " -> " #cond << '\n';                                \
            return 1;                                                         \
        }                                                                     \
    } while (0)

namespace fs = std::filesystem;

// 简易 setenv：跨平台
static void set_env(const std::string& key, const std::string& value) {
#if defined(_WIN32)
    _putenv_s(key.c_str(), value.c_str());
#else
    setenv(key.c_str(), value.c_str(), 1);
#endif
}

static fs::path make_temp_dir() {
    auto base = fs::temp_directory_path();
    std::random_device rd;
    auto dir = base / ("todo_test_" + std::to_string(rd()));
    fs::create_directories(dir);
    return dir;
}

static int test_save_load_round_trip() {
    auto dir = make_temp_dir();
    set_env("TODO_HOME", dir.string());

    todo::TaskList l;
    l.add("A");
    l.add("B");
    todo::store::save(l);

    auto got = todo::store::load();
    EXPECT(got.tasks().size() == 2);
    EXPECT(got.tasks()[0].title == "A");
    EXPECT(got.tasks()[1].title == "B");
    EXPECT(got.next_id() == 3);

    fs::remove_all(dir);
    return 0;
}

static int test_load_missing_returns_empty() {
    auto dir = make_temp_dir();
    set_env("TODO_HOME", dir.string());

    auto l = todo::store::load();
    EXPECT(l.tasks().empty());
    EXPECT(l.next_id() == 1);

    fs::remove_all(dir);
    return 0;
}

int main() {
    int rc = 0;
    rc |= test_save_load_round_trip();
    rc |= test_load_missing_returns_empty();
    if (rc == 0) std::cout << "test_store: OK\n";
    return rc;
}
