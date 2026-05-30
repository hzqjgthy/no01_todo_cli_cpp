# ============================================================
# Todo CLI (C++) - Makefile （包装 cmake 常用命令）
# 用法：make help
# ============================================================

BUILD_DIR := build
BUILD_TYPE ?= Debug
GENERATOR ?= -G "Unix Makefiles"

# Windows 下 mingw 用户可以 make GENERATOR='-G "MinGW Makefiles"'

.PHONY: help configure build run test debug clean fmt

help: ## 显示所有可用命令
	@echo "可用命令："
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN{FS=":.*?## "}{printf "  \033[36m%-12s\033[0m %s\n",$$1,$$2}'

configure: ## 配置 CMake（生成构建系统）
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(GENERATOR)

build: configure ## 编译
	cmake --build $(BUILD_DIR) -j

run: build ## 编译并运行（透传参数：make run ARGS="add 买菜"）
	./$(BUILD_DIR)/todo $(ARGS)

test: build ## 跑测试
	cd $(BUILD_DIR) && ctest --output-on-failure

debug: build ## 用 gdb 调试（示例：make debug ARGS="done 2"）
	gdb --args ./$(BUILD_DIR)/todo $(ARGS)

clean: ## 清理构建目录
	rm -rf $(BUILD_DIR)

fmt: ## 用 clang-format 格式化（需自行安装）
	@find include src tests -name '*.hpp' -o -name '*.cpp' | xargs clang-format -i
