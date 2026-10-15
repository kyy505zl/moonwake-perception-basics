# Moonwake Perception Basics

> 重庆大学 MEBC 战队 · Moonwake 视觉算法组 · 公共培训第三讲实训仓库  
> 对应讲义：《现代 C++、Python 与 CMake：把一次检测说清楚并交接给工程》  
> 仓库定位：学员实训练习基准起点仓库（Starter Repository）

---

## 📖 仓库简介

在全自主无人艇（AI Class）的感知链路中，视觉算法模块的核心职责是为下游（定位、规划、任务调度与电控）提供**及时、确定、可解释且带状态的环境观测**。

本仓库是第三讲的配套实训代码工程。我们通过一个简练而纯粹的感知核心子任务——**“输入一次浮标检测，计算它相对画面的水平偏移，并给出左/居中/右方位与有效状态”**——建立起面向真实机器人系统的工程规范：
1. **现代 C++（C++17）**：用强类型枚举、`std::optional` 和结构体说清楚检测结果，杜绝裸指针与 `-1` 魔法数；
2. **Python 自动化验收**：通过清晰的跨语言最小契约（单行 JSON + 固定退出码），用脚本自动化覆盖正常与异常边界；
3. **现代 CMake**：实现源码树与构建树彻底分离的 out-of-source 构建，让任何队友 Clone 后均可一键复现。

---

## 📂 目录结构

```text
moonwake-perception-basics/
├── CMakeLists.txt              # CMake 构建配置（定义静态库、demo 与测试）
├── README.md                   # 学习任务与操作说明（即本文档）
├── .gitignore                  # Git 忽略配置（杜绝 build/、.venv/ 等进入仓库）
├── .gitattributes              # 跨平台换行符规范
├── requirements.txt            # Python 依赖项（主要是 numpy）
├── .github/
│   └── pull_request_template.md# 规范 PR 提交模板
├── include/
│   └── moonwake/
│       ├── detection.hpp       # 语言层类型定义（Detection, BuoyOffset, BuoySide 等）
│       └── buoy_offset.hpp     # 水平偏移计算接口声明
├── src/
│   ├── buoy_offset.cpp         # [Task 01] 核心业务逻辑实现（待学员补全）
│   └── buoy_offset_demo.cpp    # 命令行测试可执行程序（输出约定 JSON）
├── scripts/
│   └── check_buoy_offset.py    # [Task 02] Python 自动化契约验收脚本
└── tests/
    └── test_buoy_offset.cpp    # [可选] C++ 单元测试
```

---

## 🛠️ 前置环境要求

- **操作系统**：Ubuntu 22.04 LTS（推荐）或配置完备的 Linux / WSL2 环境；
- **编译器**：支持 C++17 的 `g++` (>= 9.4) 或 `clang++`；
- **构建工具**：`cmake` (>= 3.16)；
- **脚本环境**：`python3` (>= 3.8) 与 `pip`。

---

## 🚀 快速开始

### 1. 配置 Python 虚拟环境与依赖

```bash
# 在仓库根目录下创建虚拟环境
python3 -m venv .venv

# 激活虚拟环境
source .venv/bin/activate

# 安装验收脚本所需的最小依赖
pip install -r requirements.txt
```

### 2. 使用 CMake 构建工程

遵循源码树与构建树分离的现代 CMake 标准方式：

```bash
# 配置构建系统（输出至 build/ 目录）
cmake -S . -B build

# 执行多核并行编译
cmake --build build -j$(nproc)
```

编译成功后，将在 `build/` 目录下生成：
- 静态库：`libmoonwake_perception.a`
- 可执行程序：`buoy_offset_demo`
- 单元测试：`test_buoy_offset`

### 3. 运行命令行 Demo

```bash
# 格式：./build/buoy_offset_demo <模式> [cx] [confidence]

# 1. 正常检测目标（中心坐标 640.0，置信度 0.92）
./build/buoy_offset_demo normal 640.0 0.92

# 2. 目标偏左
./build/buoy_offset_demo normal 400.0 0.88

# 3. 缺失目标（传入 std::nullopt）
./build/buoy_offset_demo missing

# 4. 低置信度目标（置信度 0.10 < 0.35）
./build/buoy_offset_demo low
```

### 4. 执行自动化验收脚本

```bash
python3 scripts/check_buoy_offset.py
```

---

## 🎯 任务手册：从 Task 01 到 Task 04

学员在阅读完第三讲讲义后，应基于本仓库的初始骨架逐步完成以下 4 个阶段性任务：

### Task 01｜实现检测结果与水平偏移函数

- **核心代码文件**：`src/buoy_offset.cpp`
- **任务目标**：根据 `include/moonwake/buoy_offset.hpp` 的接口定义，实现 `compute_buoy_offset` 函数。
- **业务规则与约束**：
  1. **输入防御**：当 `image_width <= 0` 或 `!det.has_value()` 时，返回默认结果（`valid = false`）；
  2. **置信度门控**：当检测置信度 `confidence < conf_threshold`（默认 0.35）时，判定为不可信，返回默认结果（`valid = false`）；
  3. **偏移计算**：
     - 图像中心横坐标：`center_x = image_width * 0.5`
     - 水平像素偏移：`dx_px = det->cx - center_x`（画面左侧为负，右侧为正）
     - 归一化偏移：`dx_norm = dx_px / center_x`（范围约在 $[-1.0, 1.0]$）
     - 填充元数据：保留 `confidence` 与观测时间戳 `stamp_ns`，并标记 `valid = true`
  4. **方位判断 (`BuoySide`)**：
     - 当 `|dx_norm| <= center_tolerance_norm`（默认 0.08）时，判定为 `BuoySide::Center`；
     - 否则当 `dx_norm < 0.0` 时判定为 `BuoySide::Left`；
     - 否则判定为 `BuoySide::Right`。
- **验收要点**：
  - 核心算法纯度：严禁直接依赖 ROS 2（保证脱离 ROS 也可单测）；
  - 杜绝旧式 C 风格：不使用 `-1` 伪造无效坐标，而是通过 `valid` 标志和 `std::optional` 表达。

---

### Task 02｜用 Python 固化跨语言契约验收

- **核心代码文件**：`scripts/check_buoy_offset.py`
- **跨语言最小契约**：
  - C++ Demo 运行后向标准输出打印**单行稳定的 JSON 字符串**；
  - 进程退出码契约：
    - `0`：成功计算出有效目标（`valid = true`）；
    - `2`：业务上无有效目标（missing 或置信度不足），**注意这是正常业务状态，非进程崩溃**；
    - `其它非 0`：参数解析失败或进程异常崩溃。
- **任务目标**：
  1. 运行 `python3 scripts/check_buoy_offset.py`，确认测试全部变绿并通过；
  2. 尝试故意改坏 `src/buoy_offset.cpp` 中的逻辑（如颠倒左右判断、改坏置信度过滤条件），观察验收脚本是否能准确指出出错用例并返回非零退出码；
  3. 恢复正确实现，记录一次通过验收的终端输出，准备填入 PR。

---

### Task 03｜用 CMake 完成可复现构建

- **任务目标**：
  1. 理解 `CMakeLists.txt` 中库目标 `add_library` 与可执行文件目标 `add_executable` 的拆分原则，以及通过 `target_link_libraries` 声明依赖；
  2. 验证全新环境可复现性：
     ```bash
     # 模拟干净目录构建
     rm -rf build/
     cmake -S . -B build
     cmake --build build
     python3 scripts/check_buoy_offset.py
     ```
  3. （可选）运行 C++ 单元测试：
     ```bash
     ctest --test-dir build --output-on-failure
     # 或者直接运行二进制：
     ./build/test_buoy_offset
     ```

---

### Task 04｜完成一次规范的团队协作分支与 PR 提交

- **任务目标**：遵循战队 Git 协作规范，开分支、写规范 Commit 并发起 Pull Request。
- **操作步骤**：
  1. **同步最新主干并创建特性分支**：
     ```bash
     git switch main
     git fetch upstream
     git merge --ff-only upstream/main
     git switch -c feat/perception-buoy-offset-cpp
     ```
  2. **在分支上完成代码实现并通过测试**；
  3. **提交前安全检查（防止污染仓库）**：
     ```bash
     git status
     # 仔细确认未暂存 Untracked 文件中绝对没有 build/、.venv/ 等临时产物！
     
     git diff
     git add include/ src/ scripts/ CMakeLists.txt README.md
     git diff --staged
     git commit -m "feat(perception): implement buoy offset logic and verification"
     ```
  4. **推送到个人 Fork 远程并提 PR**：
     ```bash
     git push -u origin feat/perception-buoy-offset-cpp
     ```
  5. **根据 `.github/pull_request_template.md` 完整填写 PR 说明**：
     - 说明修改动机；
     - 附上实际构建与 Python 检查命令及输出；
     - 确认 Checkbox 全部勾选。

---

## ⚠️ 常见误区与避坑指南

1. **绝对禁止提交 `build/` 与 `.venv/`**：构建产物带有机器环境耦合且体积庞大，推入仓库会导致严重合并冲突。
2. **严禁用 `-1` 表示无效值**：浮标可能在画面左侧（`dx_px < 0`），`-1` 是合法的坐标偏移。表示缺失请使用 `std::optional` 或 `valid = false`。
3. **不要在头文件中使用 `using namespace std;`**：这会污染下游包含者的全局命名空间，造成命名冲突。
4. **不依赖 ROS 2 头文件**：本讲专注于感知核心算法与语言契约，不引入 `rclcpp`。在后续第四讲中，我们将用 ROS 2 Node 将本库包装发布为 ROS Topic。

---

## 🚢 走向下一讲

恭喜完成第三讲！你所编写的 `moonwake_perception` 库将在第四讲《ROS 2 工程基础（一）：工作区、Package 与节点》中被直接封装为一个 ROS 2 Node，实现真正的订阅图像、发布检测与浮标偏移数据流！
