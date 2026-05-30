// main.cpp - CLI 入口与命令分发
//
// 用法：
//   todo add "买菜"
//   todo list
//   todo done <id>
//   todo rm <id>
//   todo clear
//   todo help
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "todo/store.hpp"
#include "todo/task.hpp"

namespace {

void usage() {
    std::cout <<
R"(todo - 命令行待办工具（C++ 版）

用法：
  todo add <标题>     添加任务
  todo list           列出全部任务
  todo done <id>      标记完成
  todo rm <id>        删除任务
  todo clear          清空已完成
  todo help           显示本帮助

数据文件：~/.todo/data.json （或 $TODO_HOME/data.json）
)";
}

int parse_id(const std::string& s) {
    try {
        size_t pos = 0;
        int    v   = std::stoi(s, &pos);
        if (pos != s.size()) throw std::invalid_argument("trailing");
        return v;
    } catch (const std::exception&) {
        throw std::runtime_error("id 必须是数字: " + s);
    }
}

int cmd_add(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "用法: todo add <标题>\n";
        return 1;
    }
    std::string title = args[0];
    for (size_t i = 1; i < args.size(); ++i) title += ' ' + args[i];

    auto list = todo::store::load();
    auto t    = list.add(title);
    todo::store::save(list);
    std::cout << "已添加 #" << t.id << ": " << t.title << '\n';
    return 0;
}

int cmd_list() {
    std::cerr << "cmd_list: 尚未实现\n";
    return 1;
}

int cmd_done(const std::vector<std::string>& args) {
    if (args.size() != 1) { std::cerr << "用法: todo done <id>\n"; return 1; }
    int  id   = parse_id(args[0]);
    auto list = todo::store::load();
    if (!list.mark_done(id)) {
        std::cerr << "错误: task not found (id=" << id << ")\n";
        return 1;
    }
    todo::store::save(list);
    std::cout << "已完成 #" << id << '\n';
    return 0;
}

int cmd_rm(const std::vector<std::string>& args) {
    if (args.size() != 1) { std::cerr << "用法: todo rm <id>\n"; return 1; }
    int  id   = parse_id(args[0]);
    auto list = todo::store::load();
    if (!list.remove(id)) {
        std::cerr << "错误: task not found (id=" << id << ")\n";
        return 1;
    }
    todo::store::save(list);
    std::cout << "已删除 #" << id << '\n';
    return 0;
}

int cmd_clear() {
    auto list = todo::store::load();
    int  n    = list.clear_done();
    todo::store::save(list);
    std::cout << "已清理 " << n << " 条已完成任务\n";
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) { usage(); return 2; }

    std::string              cmd = argv[1];
    std::vector<std::string> args;
    args.reserve(argc - 2);
    for (int i = 2; i < argc; ++i) args.emplace_back(argv[i]);

    try {
        if (cmd == "add")                       return cmd_add(args);
        if (cmd == "list" || cmd == "ls")       return cmd_list();
        if (cmd == "done")                      return cmd_done(args);
        if (cmd == "rm"   || cmd == "remove")   return cmd_rm(args);
        if (cmd == "clear")                     return cmd_clear();
        if (cmd == "help" || cmd == "-h" || cmd == "--help") { usage(); return 0; }

        std::cerr << "未知命令: " << cmd << "\n\n";
        usage();
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << '\n';
        return 1;
    }
}
