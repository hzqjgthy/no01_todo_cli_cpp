# no01_todo_cli_cpp 实践指南（Practice Guide）

> 🎯 **目的**：完成「学习路线第 0 层 · 03_Git 与开发工具链」的里程碑项目，把 **Git 工作流 + GDB 调试 + CMake/Makefile 构建** 三板斧串成一次完整训练。
>
> 📚 **配套文档**：
> - 顶层学习路线：[`docs/00_地基/03_Git 与开发工具链/README_易懂懂懂懂懂懂.md`](../../docs/00_地基/03_Git%20与开发工具链/README_易懂懂懂懂懂懂.md)
> - 项目说明：[`README.md`](./README.md)（含 GDB 调试剧本、推荐分支顺序）
>
> 📌 **使用方式**：每完成一节就在前面 `[ ]` 打 `[x]`，全部打完即里程碑达成。

---

## 当前状态快照

| 维度 | 现状 |
|---|---|
| 仓库 | 已托管 `https://github.com/hzqjgthy/no01_todo_cli_cpp` |
| 分支 | `main` / `develop` 已建立，工作区干净 |
| Commit | 1 个：`feat: 初始化 no01_todo_cli_cpp 脚手架` |
| 待办 | ≥10 commit · GDB 抓修 bug · 发布 v0.1.0 |

---

## 里程碑达成进度（来自 README §七）

- [x] ① 主修语言（C++）实现
- [x] ② 托管到 GitHub
- [ ] ③ ≥10 commit + 规范 message + 合理分支策略
- [x] ④ README / LICENSE / .gitignore 齐全
- [x] ⑤ CMake 管理依赖
- [x] ⑥ Makefile 收集常用命令
- [ ] ⑦ 故意造 bug 用 GDB 定位修复（流程写进 README）

---

## 阶段 0：环境准备（必做，5 分钟）

当前 TencentOS 默认未安装 `cmake`，先装上：

```bash
sudo dnf install -y cmake     # TencentOS / RHEL 系
# 或：sudo yum install -y cmake
cmake --version               # 确认 ≥ 3.16
```

跑通脚手架，确认能编译、能运行、测试能过：

```bash
cd /data/workspace/Systematic_learning/code/no01_todo_cli_cpp
make build
./build/todo help
make test
```

- [ ] `cmake` 已安装且版本 ≥ 3.16
- [ ] `make build` 成功生成 `build/todo`
- [ ] `make test` 全部通过（或至少看到 `tests/test_task` 里 SKIP 的预期跳过）

> 跑不通先修问题再继续，否则后续 GDB 调试无从谈起。

---

## 阶段 0.5：首次提交到 GitHub（10 分钟）

> ⚠️ **如果你已经完成 GitHub 托管**（远端已有 `main` 分支），跳过本节，直接进入阶段 1。

### 0.5.1 在 GitHub 网页创建空仓库

1. 浏览器打开 <https://github.com/new>
2. **Repository name**：`no01_todo_cli_cpp`
3. **Public** 或 **Private** 自选
4. ⚠️ **不要勾**「Initialize this repository with README / .gitignore / license」 ——本地已有，会冲突
5. 点 **Create repository**

建好后复制页面给出的 HTTPS 地址，例如：

```
https://github.com/<你的用户名>/no01_todo_cli_cpp.git
```

### 0.5.2 本地初始化 + 首次推送

```bash
cd /data/workspace/Systematic_learning/code/no01_todo_cli_cpp

# 初始化 git 仓库（若 .git 已存在会提示，忽略即可）
git init

# 把所有当前文件加入暂存区
git add .

# 第一次提交
git commit -m "feat: 初始化 no01_todo_cli_cpp 脚手架"

# 把默认分支改名为 main（旧版 git 默认是 master）
git branch -M main

# 关联远端仓库（用 HTTPS，不需要配 SSH key）
git remote add origin https://github.com/<你的用户名>/no01_todo_cli_cpp.git

# 第一次推送，-u 让本地 main 和远端 main 建立追踪关系
git push -u origin main
```

### 0.5.3 第一次推送的认证

`git push` 会让你输用户名 + 密码：

- **用户名**：填 GitHub 用户名
- **密码**：⚠️ 不能填登录密码（GitHub 2021 年起禁用），必须填 **Personal Access Token (PAT)**

#### 生成 PAT

1. 打开 <https://github.com/settings/tokens>
2. 点 **Generate new token → Generate new token (classic)**
3. **Note** 随便填，如 `local-push`
4. **Expiration** 选个时长（建议 90 天）
5. **Scopes** 勾选 **`repo`**
6. 拉到底部 **Generate token**
7. ⚠️ **立刻复制保存**（页面关闭就再也看不到了）

之后 `git push` 时密码栏粘贴这个 token 即可。

#### 让 Linux 记住凭证（强烈推荐）

避免每次推送都重输 PAT：

```bash
# 方式 A：内存缓存（默认 15 分钟）
git config --global credential.helper cache

# 自定义超时（如 1 小时）
git config --global credential.helper 'cache --timeout=3600'

# 方式 B：明文落盘到 ~/.git-credentials（一劳永逸）
git config --global credential.helper store
```

第一次推送输完 token 后即被保存，后续免输。

### 0.5.4 常见错误处理

| 现象 | 处理 |
|---|---|
| `fatal: remote origin already exists` | `git remote remove origin` 后重新 `add` |
| `Authentication failed` | 密码栏必须用 PAT，不是 GitHub 登录密码 |
| `error: failed to push some refs` | 远端不是空的，先 `git pull --rebase origin main` 再推；或重建一个空仓库 |
| `src refspec main does not match any` | 还没有任何 commit，先 `git add . && git commit -m "..."` |

### 0.5.5 验证

```bash
git remote -v
# origin  https://github.com/<你的用户名>/no01_todo_cli_cpp.git (fetch)
# origin  https://github.com/<你的用户名>/no01_todo_cli_cpp.git (push)

git log --oneline
git branch -a
# * main
#   remotes/origin/main
```

刷新 GitHub 仓库页面能看到代码 ✅

- [ ] GitHub 网页端建好空仓库
- [ ] 本地 `git init` + `git add` + `git commit` + `git push -u origin main` 成功
- [ ] 配好 `credential.helper`，后续推送免输 PAT
- [ ] `git remote -v` 显示正确的 origin

---

## 阶段 1：分支模型搭建（10 分钟）

采用 README §五 规划的 GitHub Flow 简化版：

```
main          ← 始终可发布，只接受来自 develop 的合并
 └── develop  ← 集成分支，所有 feat/fix/test/chore/docs 在此汇合
      ├── feat/add-list
      ├── feat/json-mini
      ├── test/store
      ├── feat/done-rm        (含故意 bug)
      ├── fix/done-id-bug     (GDB 调试后修复)  ⭐
      ├── feat/clear
      ├── chore/cmake-makefile
      └── docs/readme
```

### 1.1 创建并推送 `develop` 长期分支

```bash
# 确保当前在 main 分支且与远端同步
git checkout main
git pull origin main

# 从 main 切出 develop（已存在则跳过）
git checkout -b develop
# 若已存在：git checkout develop

# 推到远端，并设置上游追踪
git push -u origin develop
```

验证：

```bash
git branch -a
# * develop
#   main
#   remotes/origin/develop
#   remotes/origin/main
```

> 💡 **建议**：到 GitHub 仓库 → Settings → Branches → 把 `main` 设为 protected，禁止直接 push，只允许 PR 合并。这样能强制所有改动走 develop → PR → main 流程。

### 1.2 特性分支命名约定

| 前缀 | 用途 | 命名示例 |
|---|---|---|
| `feat/` | 新功能 | `feat/add-list` / `feat/json-mini` |
| `fix/` | 修 bug | `fix/done-id-bug` |
| `test/` | 加/改测试 | `test/store` |
| `chore/` | 构建、配置、依赖等杂项 | `chore/cmake-makefile` |
| `docs/` | 仅改文档 | `docs/readme` |
| `refactor/` | 不改行为的重构 | `refactor/extract-task` |

> 命名规则：**前缀小写 + 斜杠 + 短横线连接的简洁主题**，最好 1~4 个英文单词。避免中文、空格、大写。

### 1.3 每个特性分支的标准 6 步流程 ⭐

记住这个套路，后面阶段 2 的 8 个分支都是它的复制粘贴：

```bash
# ─── 步骤 1：从最新 develop 切分支 ────────────────────────
git checkout develop
git pull origin develop                  # 确保拿到最新代码
git checkout -b feat/xxx                 # 创建并切到新分支

# ─── 步骤 2：编码 + 多次小 commit ──────────────────────────
# 改完一小块就提交一次，不要一坨改完才提交
git add <文件1> <文件2>                  # 也可以用 git add -p 交互式选择
git commit -m "feat: 短描述（祈使句 ≤50 字）"
# ... 再改 ... 再提交 ...

# ─── 步骤 3：推到远端 ─────────────────────────────────────
git push -u origin feat/xxx              # 第一次推必须带 -u
# 之后再推可省 -u：git push

# ─── 步骤 4：在 GitHub 开 Pull Request ────────────────────
# 浏览器打开仓库会有 "Compare & pull request" 黄色横幅，点它
# 或手动：Pull requests → New pull request
#   - base:    develop          ⚠️ 不要选 main
#   - compare: feat/xxx
# 写好标题（同 commit message）和描述 → Create pull request

# ─── 步骤 5：自审 + 合并 ──────────────────────────────────
# Files changed 标签页过一遍 diff
# 选合并方式（推荐 "Squash and merge" 让 develop 历史更整洁）
# 点 Confirm → Delete branch（清理远端分支）

# ─── 步骤 6：同步本地 + 删本地分支 ─────────────────────────
git checkout develop
git pull origin develop                  # 把刚合并的 PR 拉下来
git branch -d feat/xxx                   # 删本地分支（已合并，安全）
# 如果还没合并就想删：git branch -D feat/xxx（强制）
```

### 1.4 常见错误处理

| 现象 | 原因 / 处理 |
|---|---|
| `fatal: A branch named 'feat/xxx' already exists` | 该分支已存在，要么 `git checkout feat/xxx` 直接进，要么 `git branch -D feat/xxx` 删了重建 |
| `error: pathspec 'develop' did not match any file...` | 本地还没 `develop`。先 `git fetch` 再 `git checkout develop` |
| `Your branch is behind 'origin/develop' by N commits` | 远端有别人/你自己的更新。先 `git pull --rebase origin develop` |
| `fatal: refusing to merge unrelated histories` | 远端是别的初始仓库。`git pull origin develop --allow-unrelated-histories`（仅一次性） |
| GitHub 上 PR 显示 "This branch has conflicts" | 远端 develop 有变化。本地 `git pull --rebase origin develop` 解决冲突后强推：`git push --force-with-lease` |
| 分支名打错想改名 | `git branch -m feat/old feat/new`，远端用 `git push origin :feat/old feat/new` |

### 1.5 阶段 1 自检

- [ ] 远端能看到 `develop` 分支
- [ ] 本地 `git branch -a` 看到 `main` / `develop` 都有 `remotes/origin/...` 对应
- [ ] 已设置 main 分支保护（可选但推荐）
- [ ] 完整记住"6 步流程"：切分支 → 提交 → 推送 → 开 PR → 合并 → 同步本地

---

## 阶段 2：按分支顺序刷 commit（核心训练，2~3 天）

完全按 README §八 的推荐顺序，每个分支至少 2~3 个 commit，自然累计 ≥10 个。

### Commit message 规范（[Conventional Commits](https://www.conventionalcommits.org/)）

| 前缀 | 用途 | 示例 |
|---|---|---|
| `feat:` | 新功能 | `feat: 实现 add/list 命令` |
| `fix:` | 修 bug | `fix: 修复 mark_done id 越界` |
| `docs:` | 文档 | `docs: 补充 GDB 调试剧本` |
| `test:` | 测试 | `test: 增加 store 往返测试` |
| `chore:` | 杂项 | `chore: 增加 install 目标` |
| `refactor:` | 重构 | `refactor: 抽出 TaskList 类` |

**首行 ≤ 50 字，祈使句现在时**；多行说明空一行后写正文（说明动机和影响）。

### 任务拆解表

| # | 分支 | 关键产出 | 建议 commit |
|---|---|---|---|
| 1 | `feat/add-list` | `todo add` + `todo list` 跑通 | `feat: 实现 add 命令` / `feat: 实现 list 命令` / `test: 加 task 单测` |
| 2 | `feat/json-mini` | `json_mini` 序列化往返通过 | `feat: 完成 json 序列化` / `feat: 完成 json 反序列化` / `test: json 往返测试` |
| 3 | `test/store` | `Store` 文件 IO 单测 | `test: store 写入读取往返` / `test: store 文件不存在场景` |
| 4 | `feat/done-rm` | `todo done` + `todo rm` 实现，**故意保留 `mark_done` 的 id 当下标 bug** | `feat: 实现 rm 命令` / `feat: 实现 done 命令（含已知 bug）` |
| 5 | **`fix/done-id-bug`** ⭐ | 用 GDB 抓修 bug —— 详见阶段 3 | `fix: 修复 mark_done id 越界 bug` |
| 6 | `feat/clear` | `todo clear` 清空已完成 | `feat: 实现 clear 命令` / `test: clear 单测` |
| 7 | `chore/cmake-makefile` | 完善构建脚本 | `chore: 加 install 目标` / `chore: 加 fmt 目标` |
| 8 | `docs/readme` | 把阶段 3 GDB 流程整理回 README §六 | `docs: 补 GDB 调试 mark_done 全流程` |

### 阶段 2 自检

- [ ] 8 个分支全部完成并合回 `develop`
- [ ] `git log --oneline` 至少 10 条
- [ ] 所有 message 都有 Conventional Commits 前缀
- [ ] GitHub 上每个分支都走过 PR 流程（不是直接 push 到 develop）

---

## 阶段 3：用 GDB 抓修 bug 全流程 ⭐（里程碑 ⑦）

这是整个里程碑**含金量最高**的训练。剧本在 README §六，本节把它落实到可执行步骤。

### 3.1 在 `feat/done-rm` 分支复现 bug

```bash
git checkout feat/done-rm
make build

# 准备触发 bug 的数据
rm -rf ~/.todo               # 清干净
./build/todo add "A"         # 得到 #1
./build/todo add "B"         # 得到 #2
./build/todo rm 1            # 删 #1，只剩 #2 (B)
./build/todo done 2          # 期望成功，实际报错
# → "task not found"，但 #2 明明在
```

**保留终端输出**，后续要贴进 README。

- [ ] bug 已复现，错误输出已截图/复制保存

### 3.2 切到修复分支并用 GDB 定位

```bash
# 从 develop（注意：不是从 feat/done-rm）切修复分支
# 但需要把 feat/done-rm 的 bug 代码拿过来 —— 简单做法：feat/done-rm 已合并到 develop 后再切
# 如果 feat/done-rm 尚未合并，则直接：
git checkout -b fix/done-id-bug feat/done-rm

# 重新准备数据
rm -rf ~/.todo
./build/todo add "A"
./build/todo add "B"
./build/todo rm 1

# 启动 GDB（Makefile 已封装）
make debug ARGS="done 2"
# 或：gdb --args ./build/todo done 2
```

进入 `(gdb)` 后按剧本走：

```text
(gdb) break todo::TaskList::mark_done    # 在 mark_done 入口下断点
(gdb) run                                # 触发，命中断点

(gdb) print id                           # 看入参 → 应显示 2
(gdb) print tasks_.size()                # 看容器大小 → 应显示 1
(gdb) print tasks_                       # 看整个 vector 内容
(gdb) list                               # 看代码上下文，确认是 tasks_[id] 越界
(gdb) bt                                 # 打印调用栈
(gdb) next                               # 单步走一行
(gdb) step                               # 步入函数
(gdb) continue                           # 继续到结束

(gdb) quit                               # 退出
```

🎯 **结论**：`id=2` 但 `tasks_.size()=1`，代码却按 `tasks_[id]` 取下标 → 越界访问 / 走错分支。

**逐条复制 GDB 输出**到剪贴板，准备阶段 4 写进 README。

- [ ] 成功在 `mark_done` 设断点
- [ ] 用 `print` 查看了 `id` / `tasks_` / `tasks_.size()`
- [ ] 用 `bt` 看到调用栈
- [ ] 至少用过一次 `next` 或 `step` 单步

### 3.3 进阶（可选但强烈推荐）：core dump 事后调试

完成自检清单"能分析 core dump"：

```bash
# 1) 临时把 mark_done 的越界检查注释掉，让它直接 tasks_[id] 触发 UB
# 2) 允许生成 core
ulimit -c unlimited
echo "core.%p" | sudo tee /proc/sys/kernel/core_pattern    # 可选，控制 core 路径

# 3) 触发崩溃
./build/todo done 999

# 4) 事后调试
gdb ./build/todo core.<pid>
(gdb) bt                # 看崩在哪
(gdb) frame 0           # 跳到 0 号栈帧
(gdb) print id
(gdb) quit
```

完成后**记得把代码改回去**（恢复越界检查），不要把这次实验提交。

- [ ] （可选）完成 core dump 分析

### 3.4 修复 + 启用回归测试

修改 `src/task.cpp` 的 `mark_done`，改为按 `id` 线性查找：

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

打开 `tests/test_task.cpp` 里的回归测试（一般是把 `#define SKIP 1` 改成 `#define SKIP 0`）。

```bash
make test    # 应全绿
```

### 3.5 提交修复

```bash
git add src/task.cpp tests/test_task.cpp
git commit -m "fix: 修复 mark_done 把 id 当下标的越界 bug

通过 GDB 在 task.cpp:mark_done 单步定位到 id 与下标混用，
改为按 Task::id 线性查找，并启用回归测试 test_mark_done_by_id。

复现步骤：
  todo add A; todo add B; todo rm 1; todo done 2
  原本应成功，但报 'task not found'。
"

git push origin fix/done-id-bug
# 到 GitHub 开 PR → 合到 develop
```

### 3.6 把 GDB 流程写回 README

打开 `README.md` §六，把你 3.2 复制下来的真实 GDB 输出替换 / 补充进去（包括 `print id` / `print tasks_` 等真实数值）。这一步在 `docs/readme` 分支做（见阶段 2 任务表 #8）。

- [ ] `fix/done-id-bug` 已 PR 合并
- [ ] `make test` 全绿
- [ ] README §六 已更新为真实 GDB 输出
- [ ] 提交了带有详细 body 的 commit message

---

## 阶段 4：发布 v0.1.0（30 分钟）

所有分支合完后收官。

```bash
# 1) develop → main
git checkout main && git pull origin main
git merge --no-ff develop -m "release: v0.1.0 完成第 0 层里程碑"
git push origin main

# 2) 打带签注的 tag
git tag -a v0.1.0 -m "里程碑：协作式 ToDo CLI（C++ 版）

- Git 工作流：main / develop / feat-fix-test-chore-docs 多分支
- GDB 调试：mark_done id 越界 bug 完整定位与修复流程
- CMake + Makefile：跨平台构建
- ≥10 commit · Conventional Commits"
git push origin v0.1.0
```

去 GitHub Releases 页面，基于 `v0.1.0` 创建一个 Release：
- **Title**：`v0.1.0 — 协作式 ToDo CLI 里程碑`
- **Body**：写两段总结（练了什么 / 踩了什么坑 / 下一步去网络编程层），附一张 GDB 截图

- [ ] `main` 分支与 `develop` 同步
- [ ] tag `v0.1.0` 已推送
- [ ] GitHub Release 已发布

---

## 阶段 5：补全 Git 高级技能（自检清单兜底）

文档自检清单里还要求 **rebase / 解决冲突 / stash / cherry-pick**，建议在阶段 2 中途穿插练习，否则收官后单独补一节。

### 5.1 练 `rebase`

在某个 feat 分支上：

```bash
git checkout feat/clear
git fetch origin
git rebase origin/develop                     # 把 develop 最新提交垫到自己脚下
# 出现冲突 → 改文件 → git add → git rebase --continue
git push --force-with-lease origin feat/clear # rebase 后必须强推
```

### 5.2 故意造一次冲突并解决

```bash
git checkout develop
echo "A 行" >> NOTES.md && git add . && git commit -m "docs: 加 A"
git checkout -b feat/conflict-demo HEAD~1
echo "B 行" >> NOTES.md && git add . && git commit -m "docs: 加 B"
git rebase develop
# 此时冲突 → 编辑 NOTES.md 解决 → git add → git rebase --continue
```

### 5.3 `stash` 与 `cherry-pick`

```bash
# stash：调试中临时保存改动
git stash push -m "wip: 调到一半"
git checkout main
# ... 处理完
git checkout feat/xxx
git stash pop

# cherry-pick：把某个 commit 挑到另一分支
git checkout develop
git cherry-pick <fix/done-id-bug 上的 commit hash>
```

- [ ] 完成过一次 rebase
- [ ] 主动制造并解决了一次冲突
- [ ] 用过 `git stash push` / `pop`
- [ ] 用过 `git cherry-pick`

---

## 全量自检清单（来自 `docs/00_地基/03_Git 与开发工具链/README_易懂懂懂懂懂懂.md`）

### Git
- [ ] 熟练 `add` / `commit` / `push` / `pull`
- [ ] 会用 `branch` / `merge` / `rebase`
- [ ] 能解决冲突
- [ ] 用过 `stash` / `cherry-pick` / `reset` / `revert`
- [ ] 懂 GitHub Flow（本项目即采用）
- [ ] 会用 `.gitignore`、`tag`

### 调试
- [ ] 会用 GDB 设断点（`break`）、单步（`next`/`step`）、查变量（`print`）、看栈（`bt`）
- [ ] 能分析 core dump（阶段 3.3 可选项）
- [ ] （Go 项目里）会用 Delve 调试 —— 已在 `no01_todo_cli_go` 中练过

### 构建
- [x] 能写 `CMakeLists.txt` 管理多文件 C++ 项目（脚手架已具备）
- [x] 会写 `Makefile` 收集常用命令（脚手架已具备）

---

## 进度追踪面板

```
阶段 0    环境准备           [ ]
阶段 0.5  GitHub 首次推送   [ ]
阶段 1    分支模型           [ ]
阶段 2  feat/add-list      [ ]
        feat/json-mini     [ ]
        test/store         [ ]
        feat/done-rm       [ ]
        fix/done-id-bug ⭐ [ ]
        feat/clear         [ ]
        chore/cmake...     [ ]
        docs/readme        [ ]
阶段 3  GDB 全流程          [ ]
        core dump（选）     [ ]
阶段 4  v0.1.0 发布         [ ]
阶段 5  rebase/冲突/stash   [ ]
        /cherry-pick
```

每完成一项把 `[ ]` 改成 `[x]`，全勾代表里程碑完美达成 ✅

---

## 一句话总结

> **节奏**：装 cmake → 建 GitHub 仓库并首次 push → 推 develop → 8 个分支 ≥10 commit → GDB 抓修 bug → 发 v0.1.0。
>
> **关键**：不是为了完成任务而走流程，而是借这个小项目把"代码 → 测试 → 调试 → 提交 → 发布"的闭环走熟，这是后续 L2 网络编程、L7 完整后台项目都要反复用的底层动作。
