## 1. 为什么修改 (Why)
<!-- 简要说明本 PR 的动机、解决的问题以及完成的学习任务 (如完成 Task 01 ~ Task 04) -->

## 2. 改动清单 (Changed Files)
<!-- 列出本次 PR 修改或新增的核心文件及改动说明 -->
- `src/buoy_offset.cpp`: 
- `scripts/check_buoy_offset.py`: 
- `README.md`: 

## 3. 验证环境与执行命令 (Verification Environment & Commands)
<!-- 说明你的测试环境（系统版本、编译器、Python 版本）及完整执行命令 -->
- **系统环境**: Ubuntu 22.04 LTS / GCC 11.4 / Python 3.10
- **复现命令**:
  ```bash
  cmake -S . -B build
  cmake --build build
  python3 scripts/check_buoy_offset.py
  ```

## 4. 实际观察到的结果 (Observed Results)
<!-- 粘贴运行 check_buoy_offset.py 脚本的实际终端输出（包含用例通过信息） -->
```text
[INFO] 正在对可执行文件进行契约验收: ...
✅ CHECK PASSED: 正常用例 / 缺失用例 / 低置信度用例 / 边界容差用例 均已通过！
```

## 5. 尚未覆盖的场景与边界说明 (Uncovered Edge Cases)
<!-- 说明当前算法或测试尚未覆盖的边界，例如极大极小尺寸目标、畸变严重、光照异常等 -->

## 6. 对后续 ROS 2 接口或系统模块的影响 (Impact on Downstream / ROS 2)
<!-- 说明本讲语言层类型 (Detection / BuoyOffset) 未来接入 ROS 2 Node 时的转换思路与边界隔离 -->

---

### 提交前自查清单 (Checklist)
- [ ] 没有将 `build/`、`.venv/`、可执行文件或 IDE 配置文件提交到 Git
- [ ] 遵循 Conventional Commits 提交规范（如 `feat(perception): add buoy offset logic`）
- [ ] 核心计算逻辑不依赖 ROS 2（保持纯粹的语言层算法库）
- [ ] 没有使用 `-1` 伪造无效坐标，而是通过 `valid` 标志或 `std::optional` 表达
- [ ] 所有自动化验收用例均在本地成功跑通 (Exit Code = 0)
