// tests/test_buoy_offset.cpp
#include "moonwake/buoy_offset.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

using moonwake::BuoyOffset;
using moonwake::BuoySide;
using moonwake::Detection;
using moonwake::ObjectClass;
using moonwake::compute_buoy_offset;

static bool approx(double a, double b, double tol = 1e-4) {
  return std::abs(a - b) <= tol;
}

int main() {
  std::cout << "[INFO] Running buoy_offset C++ unit tests...\n";

  const int width = 1280;

  // 1. 正常居中用例: cx = 640.0
  {
    Detection d;
    d.cx = 640.0;
    d.confidence = 0.90;
    auto res = compute_buoy_offset(d, width);
    assert(res.valid && "Center case should be valid");
    assert(res.side == BuoySide::Center && "Expected side == Center");
    assert(approx(res.dx_px, 0.0) && "Expected dx_px == 0.0");
    assert(approx(res.dx_norm, 0.0) && "Expected dx_norm == 0.0");
    assert(approx(res.confidence, 0.90));
  }

  // 2. 偏左用例: cx = 400.0 (dx_px = -240.0, dx_norm = -240 / 640 = -0.375)
  {
    Detection d;
    d.cx = 400.0;
    d.confidence = 0.85;
    auto res = compute_buoy_offset(d, width);
    assert(res.valid && "Left case should be valid");
    assert(res.side == BuoySide::Left && "Expected side == Left");
    assert(approx(res.dx_px, -240.0) && "Expected dx_px == -240.0");
    assert(approx(res.dx_norm, -0.375) && "Expected dx_norm == -0.375");
  }

  // 3. 偏右用例: cx = 880.0 (dx_px = +240.0, dx_norm = +0.375)
  {
    Detection d;
    d.cx = 880.0;
    d.confidence = 0.85;
    auto res = compute_buoy_offset(d, width);
    assert(res.valid && "Right case should be valid");
    assert(res.side == BuoySide::Right && "Expected side == Right");
    assert(approx(res.dx_px, 240.0) && "Expected dx_px == 240.0");
    assert(approx(res.dx_norm, 0.375) && "Expected dx_norm == 0.375");
  }

  // 4. 边界容差用例: tolerance = 0.08, center_x = 640
  //    容差像素范围: 640 * 0.08 = 51.2 像素
  //    cx = 640 + 50 = 690 -> dx_norm = 50 / 640 = 0.078125 <= 0.08 -> Center
  {
    Detection d;
    d.cx = 690.0;
    d.confidence = 0.80;
    auto res = compute_buoy_offset(d, width);
    assert(res.valid);
    assert(res.side == BuoySide::Center && "cx=690 should fall within center tolerance");
  }

  // 5. 缺失目标: std::nullopt
  {
    auto res = compute_buoy_offset(std::nullopt, width);
    assert(!res.valid && "Missing detection must return valid = false");
    assert(res.side == BuoySide::Invalid);
  }

  // 6. 低置信度目标: conf = 0.10 < 0.35
  {
    Detection d;
    d.cx = 640.0;
    d.confidence = 0.10;
    auto res = compute_buoy_offset(d, width);
    assert(!res.valid && "Low confidence must return valid = false");
    assert(res.side == BuoySide::Invalid);
  }

  // 7. 非法图像宽度: width <= 0
  {
    Detection d;
    d.cx = 100.0;
    d.confidence = 0.90;
    auto res1 = compute_buoy_offset(d, 0);
    auto res2 = compute_buoy_offset(d, -640);
    assert(!res1.valid && "Width 0 must be invalid");
    assert(!res2.valid && "Width < 0 must be invalid");
  }

  std::cout << "[PASS] All C++ unit test assertions passed successfully!\n";
  return 0;
}
