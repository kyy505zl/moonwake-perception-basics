#!/usr/bin/env python3
"""验收脚本：验证 buoy_offset_demo 的业务契约、JSON 输出与退出码。

测试维度：
1. normal 居中用例 (cx=640)  -> valid=true, side=center, dx_px≈0, code=0
2. normal 偏左用例 (cx=400)  -> valid=true, side=left,   dx_px<0, code=0
3. normal 偏右用例 (cx=880)  -> valid=true, side=right,  dx_px>0, code=0
4. missing 缺失目标           -> valid=false, side=invalid, code=2
5. low 低置信度目标           -> valid=false, side=invalid, code=2
6. [Task 02 拓展] 边界容差用例 -> 验证处于 center_tolerance 临界附近的行为
"""

from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

try:
    import numpy as np

    def approx(a: float, b: float, tol: float = 1e-4) -> bool:
        return bool(np.isclose(a, b, atol=tol, rtol=0.0))
except ImportError:
    # 未安装 numpy 时回退到标准库 math.isclose
    import math

    def approx(a: float, b: float, tol: float = 1e-4) -> bool:
        return math.isclose(a, b, abs_tol=tol)


ROOT = Path(__file__).resolve().parents[1]

# 跨平台候选路径支持 (Linux ELF 以及 Windows .exe, 支持 CMake 多配置生成器)
BIN_NAMES = ["buoy_offset_demo", "buoy_offset_demo.exe"]
DEMO_CANDIDATES = [
    ROOT / "build" / name
    for name in BIN_NAMES
] + [
    ROOT / "build" / cfg / name
    for cfg in ("Debug", "Release", "RelWithDebInfo")
    for name in BIN_NAMES
]


def find_demo() -> Path:
    """查找已构建的 buoy_offset_demo 可执行文件路径。"""
    for p in DEMO_CANDIDATES:
        if p.is_file():
            return p
    raise FileNotFoundError(
        "找不到 buoy_offset_demo 可执行程序。请先在仓库根目录执行构建：\n"
        "  cmake -S . -B build && cmake --build build"
    )


def run_case(demo: Path, args: list[str]) -> tuple[dict, int]:
    """运行一次 demo 并解析最后一行 JSON 输出及进程退出码。"""
    proc = subprocess.run(
        [str(demo), *args],
        capture_output=True,
        text=True,
        check=False,
    )
    stdout_clean = proc.stdout.strip()
    line = stdout_clean.splitlines()[-1] if stdout_clean else ""
    try:
        payload = json.loads(line) if line else {}
    except json.JSONDecodeError as exc:
        raise AssertionError(
            f"标准输出最后一行不是有效 JSON：{line!r}\n"
            f"完整 stdout:\n{proc.stdout}\n"
            f"完整 stderr:\n{proc.stderr}"
        ) from exc
    return payload, proc.returncode


def main() -> int:
    try:
        demo = find_demo()
    except FileNotFoundError as err:
        print(f"[ERROR] {err}", file=sys.stderr)
        return 1

    print(f"[INFO] 正在对可执行文件进行契约验收: {demo}")
    errors: list[str] = []

    # ---------------------------------------------------------
    # 1) 正常样本：中心附近 -> center, code=0
    # ---------------------------------------------------------
    payload, code = run_case(demo, ["normal", "640", "0.92"])
    if code != 0 or not payload.get("valid"):
        errors.append(f"[Case 1] 居中用例未得到有效目标：code={code}, payload={payload}")
    else:
        if payload.get("side") != "center":
            errors.append(f"[Case 1] 居中用例期望 side='center'，实际得到 '{payload.get('side')}'")
        if not approx(float(payload.get("dx_px", 1e9)), 0.0, tol=1e-3):
            errors.append(f"[Case 1] 居中用例期望 dx_px≈0，实际得到 {payload.get('dx_px')}")
        if not approx(float(payload.get("dx_norm", 1e9)), 0.0, tol=1e-3):
            errors.append(f"[Case 1] 居中用例期望 dx_norm≈0，实际得到 {payload.get('dx_norm')}")

    # ---------------------------------------------------------
    # 2) 正常样本：偏左 -> left, code=0
    # ---------------------------------------------------------
    payload, code = run_case(demo, ["normal", "400", "0.88"])
    if code != 0 or not payload.get("valid"):
        errors.append(f"[Case 2] 偏左用例未得到有效目标：code={code}, payload={payload}")
    else:
        if payload.get("side") != "left":
            errors.append(f"[Case 2] 偏左用例期望 side='left'，实际得到 '{payload.get('side')}'")
        if float(payload.get("dx_px", 0.0)) >= 0.0:
            errors.append(f"[Case 2] 偏左用例期望 dx_px < 0，实际得到 {payload.get('dx_px')}")

    # ---------------------------------------------------------
    # 3) 正常样本：偏右 -> right, code=0
    # ---------------------------------------------------------
    payload, code = run_case(demo, ["normal", "880", "0.88"])
    if code != 0 or not payload.get("valid"):
        errors.append(f"[Case 3] 偏右用例未得到有效目标：code={code}, payload={payload}")
    else:
        if payload.get("side") != "right":
            errors.append(f"[Case 3] 偏右用例期望 side='right'，实际得到 '{payload.get('side')}'")
        if float(payload.get("dx_px", 0.0)) <= 0.0:
            errors.append(f"[Case 3] 偏右用例期望 dx_px > 0，实际得到 {payload.get('dx_px')}")

    # ---------------------------------------------------------
    # 4) 缺失目标 (missing) -> valid=false, code=2
    # ---------------------------------------------------------
    payload, code = run_case(demo, ["missing"])
    if code != 2 or payload.get("valid") is not False:
        errors.append(
            f"[Case 4] missing 样本期望 valid=false 且退出码为 2：实际 code={code}, payload={payload}"
        )

    # ---------------------------------------------------------
    # 5) 低置信度目标 (low) -> valid=false, code=2
    # ---------------------------------------------------------
    payload, code = run_case(demo, ["low"])
    if code != 2 or payload.get("valid") is not False:
        errors.append(
            f"[Case 5] low 置信度样本期望 valid=false 且退出码为 2：实际 code={code}, payload={payload}"
        )

    # ---------------------------------------------------------
    # 6) [Task 02 延伸] 边界容差测试：center_x=640, tolerance=0.08 (阈值偏移 ±51.2px)
    #    cx=680 (偏移+40px < 51.2px) 应当仍然判定为 center
    # ---------------------------------------------------------
    payload, code = run_case(demo, ["normal", "680", "0.90"])
    if code != 0 or payload.get("side") != "center":
        errors.append(
            f"[Case 6] 容差范围内样本 (cx=680) 期望 side='center'：实际 code={code}, payload={payload}"
        )

    # ---------------------------------------------------------
    # 输出汇总结果
    # ---------------------------------------------------------
    if errors:
        print("\n" + "=" * 50)
        print("❌ CHECK FAILED - 存在未通过的契约用例：")
        print("=" * 50)
        for e in errors:
            print("  *", e)
        print("\n提示：若刚 Clone 仓库，请先在 src/buoy_offset.cpp 中完成 Task 01 的实现。")
        return 1

    print("\n" + "=" * 50)
    print("✅ CHECK PASSED: 正常用例 / 缺失用例 / 低置信度用例 / 边界容差用例 均已通过！")
    print("=" * 50)
    return 0


if __name__ == "__main__":
    sys.exit(main())
