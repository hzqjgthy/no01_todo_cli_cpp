# no01_todo_cli_cpp — 协作式 ToDo 管理工具（C++ 版）

> 学习路线第 0 层 · 03_Git 与开发工具链 的里程碑项目（C++ 版）。
>
> 与 `no01_todo_cli_go` 是**同一个里程碑的双语言实现**，便于横向对比 Go / C++ 的工程化差异。
>
> 目标：把 **Git 工作流 + GDB 调试 + CMake / Makefile 构建** 串成一个可跑通的小项目。

---

## 一、功能（与 Go 版对齐）

```bash
todo add "买菜"     # 添加任务
todo list           # 列出全部
todo done 1         # 标记完成
todo rm 1           # 删除
todo clear          # 清空已完成
todo help           # 帮助
```

数据存储在 `~/.todo/data.json`，可通过 `TODO_HOME` 环境变量覆盖。

> Windows 上路径为 `%USERPROFILE%\.todo\data.json`（仅作参考，本仓库当前在 Linux 上运行）。

> ⚠️ 与 Go 版**共享同一份数据文件**（同一个 JSON 结构）。如果同时安装两个版本，操作会互相影响。

---

## 二、快速开始

### 2.1 依赖

| 工具 | 最低版本 | 说明 |
|---|---|---|
| **CMake** | 3.16 | 构建系统 |
| **C++ 编译器** | 支持 C++17 | g++ 7+ / clang++ 5+ |
| **GDB**（可选） | 任意 | 调试演示用 |
| **Make**（可选） | 任意 | 包装常用 cmake 命令 |

> **当前环境（TencentOS / Linux）**：`g++` / `gcc` / `make` / `gdb` 已安装；`cmake` 默认未安装，需先执行 `sudo dnf install -y cmake`（或 `yum install -y cmake`）。

### 2.2 编译运行（Linux / macOS / WSL）

```bash
# 方式 A：Makefile 一把梭
make build
./build/todo add "学习 GDB"
./build/todo list
make test

# 方式 B：纯 cmake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/todo list
cd build && ctest --output-on-failure
```

<details>
<summary>Windows + Visual Studio 用户（仅供参考，本仓库当前在 Linux 上运行）</summary>

```powershell
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\todo.exe list
```

</details>

---

## 三、项目结构

```
no01_todo_cli_cpp/
├── include/todo/              # 公共头文件
│   ├── task.hpp               # Task / TaskList
│   ├── store.hpp              # 持久化接口
│   └── json_mini.hpp          # 极简 JSON 序列化
├── src/
│   ├── task.cpp               # ⚠️ mark_done 内置故意 bug
│   ├── store.cpp              # 文件 IO（原子写）
│   ├── json_mini.cpp          # 手写 JSON 解析
│   └── main.cpp               # CLI 入口
├── tests/
│   ├── CMakeLists.txt
│   ├── test_task.cpp
│   └── test_store.cpp
├── CMakeLists.txt
├── Makefile                   # 包装 cmake 常用命令
├── LICENSE
├── .gitignore
└── README.md
```

**分层原则**：
- `include/todo/*.hpp`：对外接口
- `src/main.cpp`：只做参数解析 / 输出
- `src/task.cpp`：纯业务逻辑，零文件 IO
- `src/store.cpp` + `json_mini.cpp`：持久化
- 三层互不越权，便于单测

---

## 四、关键设计选择

| 维度 | 选择 | 为什么不选另一个 |
|---|---|---|
| C++ 标准 | **C++17** | C++14 没 `std::filesystem`、`optional`；C++20 模块化支持还不普及 |
| JSON | **手写极简解析器** | 不引入 nlohmann/json 是为了练 C++ 基本功 + 体现"零依赖"。结构稳定后真要扩展再换三方库 |
| 测试 | **CTest + 自写断言** | 不引入 GoogleTest。CTest 是 CMake 自带，零依赖 |
| 错误处理 | **异常 + 返回值组合** | 业务函数（add/remove）返回 bool；IO/解析错误抛 `std::runtime_error` |
| 时间戳 | **int64 unix seconds** | 与 Go 版 `time.Time` JSON 序列化兼容（同一份 data.json） |

---

## 五、Git 工作流

跟 Go 版**完全一致**，建议起这些分支：

```
main
 └── develop
      ├── feat/init             (脚手架)
      ├── feat/add-list         (add + list)
      ├── feat/json-mini        (手写 JSON)
      ├── feat/done-rm          (含故意 bug)
      ├── fix/done-id-bug       (GDB 调试后修复)
      ├── feat/clear            (clear)
      ├── chore/cmake-makefile  (构建优化)
      ├── docs/readme           (完善文档)
      └── test/                 (补单元测试)
```

Commit message 用 [Conventional Commits](https://www.conventionalcommits.org/)：`feat / fix / docs / chore / test / refactor`。

---

## 六、用 GDB 抓 bug 的完整流程 🔍

> 这是本项目设计的"故意 bug"，**对应 Go 版用 Delve 调试的相同 bug**，让你练 GDB。

### 6.1 复现 bug

```bash
make build
./build/todo add "A"
./build/todo add "B"
./build/todo rm 1     # 删掉 #1，只剩 #2 (B)
./build/todo done 2   # 试着完成它
# → 报错 "task not found"，但 #2 明明存在！
```

### 6.2 用 GDB 定位

```bash
# Makefile 提供了快捷方式：make debug ARGS="done 2"
gdb --args ./build/todo done 2
```

进入 `(gdb)` 提示符后：

```
(gdb) break todo::TaskList::mark_done
Breakpoint 1 at ...

(gdb) run
... 命中断点 ...

(gdb) print id
$1 = 2

(gdb) print tasks_.size()
$2 = 1

(gdb) print tasks_
$3 = std::vector of length 1, capacity 1 = {{
    id = 2, title = "B", done = false, created_at = ...
}}

(gdb) list
... 看到代码用 tasks_[id] 取下标 ...
```

🎯 **真相**：`id=2` 但 `tasks_.size()=1`，代码却把 `id` 当下标 → 走 `id >= size` 分支返回 false。

> 💡 想看更刺激的：把 `mark_done` 里的越界检查注释掉，直接 `tasks_[id]`，会触发 **未定义行为** —— 在 Linux 上配合 `ulimit -c unlimited` 还能 dump core，再用 `gdb ./build/todo core` 做事后调试。这是 GDB 经典训练。

### 6.3 修复

把 `src/task.cpp` 里的 `mark_done` 改为按 ID 线性查找：

```cpp
bool TaskList::mark_done(int id) {
    for (auto& t : tasks_) {
        if (t.id == id) {
            t.done = true;
            return true;
        }
    }
    return false;
}
```

同时把 `tests/test_task.cpp` 里 `#define SKIP 1` 改成 `#define SKIP 0`，让回归测试生效。

### 6.4 验证 + 提交

```bash
make test    # 所有用例应通过

git checkout -b fix/done-id-bug
git add src/task.cpp tests/test_task.cpp
git commit -m "fix: 修复 mark_done 把 id 当下标的越界 bug

通过 GDB 单步定位到 task.cpp:mark_done 中 id 与下标混用，
改为按 Task::id 线性查找，并启用回归测试。"
git push origin fix/done-id-bug
```

---

## 七、自检清单（对照里程碑要求）

- [x] ① 主修语言（C++）实现
- [ ] ② 托管到 GitHub，分支策略合理 ← 见 §九 推送方法
- [ ] ③ ≥10 commit，message 规范 ← 按 §五 分支顺序提交
- [x] ④ README / LICENSE / .gitignore 齐全
- [x] ⑤ CMake 管理依赖
- [x] ⑥ Makefile 收集常用命令
- [ ] ⑦ 故意造 bug 用 GDB 定位修复 ← 按 §六 走完一遍

---

## 八、推荐学习顺序（建议每步一个 commit）

1. `feat/init`：初始化骨架（当前状态）
2. `feat/add-list`：先把 `add` + `list` 跑通
3. `feat/json-mini`：完成 JSON 序列化 + 单测
4. `test/store`：补 store 往返测试
5. `feat/done-rm`：加 `done` + `rm`（**故意留 bug**）
6. `fix/done-id-bug`：用 GDB 调试 + 修复（流程写进 README）
7. `feat/clear`：实现 `clear`
8. `chore/cmake-makefile`：完善构建脚本
9. `docs/readme`：完善文档
10. 合并 `develop → main`，打 tag `v0.1.0`

---

## 九、附录 A：如何托管到 GitHub（HTTPS 方式）

> 仓库名建议：`no01_todo_cli_cpp`（与 Go 版的 `no01_todo_cli_go` 区分开）

### A.1 在 GitHub 网页端建空仓库

1. 浏览器打开 <https://github.com/new>
2. **Repository name**：`no01_todo_cli_cpp`
3. Public / Private 自选
4. **不要勾**初始化 README / .gitignore / license
5. 创建

### A.2 本地推送

```bash
cd /data/workspace/Systematic_learning/code/no01_todo_cli_cpp

git init
git add .
git commit -m "feat: 初始化 no01_todo_cli_cpp 脚手架"
git branch -M main

git remote add origin https://github.com/<你的用户名>/no01_todo_cli_cpp.git
git push -u origin main
```

第一次推送会要求输用户名 + **PAT**（不是登录密码）。生成 PAT 的步骤、保存凭证、常见报错处理，**与 Go 版 README 附录 A 完全一致**，参见：

`../no01_todo_cli_go/README.md` 的 **附录 A：如何托管到 GitHub**

---

## 十、与 Go 版的横向对比 📊

| 维度 | Go 版 | C++ 版 |
|---|---|---|
| 代码量 | ~250 行 | ~600 行（多了 JSON / 头文件 / CMake） |
| 依赖管理 | `go mod`（一行 `module …`） | `CMakeLists.txt`（35 行） |
| JSON | `encoding/json`（标准库） | 手写 90 行迷你解析器 |
| 测试框架 | `go test`（标准库） | CTest + 自写宏 |
| 调试器 | Delve (`dlv`) | GDB |
| 跨平台编译 | `GOOS=linux go build` 一条命令 | 需要交叉编译工具链 |
| 错误处理 | 多返回值 `(T, error)` | 异常 + bool 返回 |
| 内存管理 | GC | RAII（`std::vector` / `std::string` 自动释放） |

**结论**：Go 适合快速搭工具，C++ 适合精确控制 + 工业级框架。本里程碑做完两版后，你会切身感受到工程化复杂度的差异，这正是后续学 muduo（C++）/ skynet（Lua+C）/ Nano（Go）时最需要的体感。

---

## License

MIT © 2026 aippletian
