#include "todo/store.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "todo/json_mini.hpp"

namespace fs = std::filesystem;

namespace todo::store {

namespace {

constexpr const char* kDataFileName = "data.json";

fs::path home_dir() {
    if (const char* env = std::getenv("TODO_HOME"); env && *env) {
        return fs::path(env);
    }
#if defined(_WIN32)
    if (const char* up = std::getenv("USERPROFILE"); up && *up) {
        return fs::path(up) / ".todo";
    }
#else
    if (const char* home = std::getenv("HOME"); home && *home) {
        return fs::path(home) / ".todo";
    }
#endif
    throw std::runtime_error("cannot determine home directory");
}

}  // namespace

fs::path data_path() {
    return home_dir() / kDataFileName;
}

TaskList load() {
    auto p = data_path();
    if (!fs::exists(p)) return TaskList{};  // 首次运行

    std::ifstream ifs(p);
    if (!ifs) throw std::runtime_error("cannot open: " + p.string());
    std::ostringstream oss;
    oss << ifs.rdbuf();

    TaskList list;
    json_mini::parse(oss.str(), list);
    return list;
}

void save(const TaskList& list) {
    auto p   = data_path();
    auto dir = p.parent_path();
    fs::create_directories(dir);

    auto tmp = p;
    tmp += ".tmp";

    {
        std::ofstream ofs(tmp, std::ios::binary | std::ios::trunc);
        if (!ofs) throw std::runtime_error("cannot write: " + tmp.string());
        ofs << json_mini::dump(list);
    }
    fs::rename(tmp, p);
}

}  // namespace todo::store
