// src/buoy_offset_demo.cpp
#include "moonwake/buoy_offset.hpp"

#include <iostream>
#include <optional>
#include <string>

using moonwake::BuoyOffset;
using moonwake::BuoySide;
using moonwake::Detection;
using moonwake::ObjectClass;
using moonwake::compute_buoy_offset;

static const char* side_to_cstr(BuoySide s) {
  switch (s) {
    case BuoySide::Left:
      return "left";
    case BuoySide::Center:
      return "center";
    case BuoySide::Right:
      return "right";
    default:
      return "invalid";
  }
}

int main(int argc, char** argv) {
  // 用法说明：
  //   buoy_offset_demo missing
  //   buoy_offset_demo low
  //   buoy_offset_demo normal <cx> <conf>
  const int image_width = 1280;
  std::string mode = (argc >= 2) ? argv[1] : "normal";

  std::optional<Detection> det;

  if (mode == "missing") {
    det = std::nullopt;
  } else if (mode == "low") {
    Detection d;
    d.class_id = ObjectClass::GreenBuoy;
    d.cx = 400.0;
    d.cy = 360.0;
    d.width = 40.0;
    d.height = 60.0;
    d.confidence = 0.10;  // 低于默认阈值 0.35
    d.stamp_ns = 1'000'000;
    det = d;
  } else {
    Detection d;
    d.class_id = ObjectClass::GreenBuoy;
    d.cx = (argc >= 3) ? std::stod(argv[2]) : 640.0;
    d.cy = 360.0;
    d.width = 50.0;
    d.height = 70.0;
    d.confidence = (argc >= 4) ? std::stod(argv[3]) : 0.92;
    d.stamp_ns = 2'000'000;
    det = d;
  }

  BuoyOffset off = compute_buoy_offset(det, image_width);

  // 稳定、单行 JSON 输出，供 scripts/check_buoy_offset.py 解析
  std::cout << "{"
            << "\"valid\":" << (off.valid ? "true" : "false") << ","
            << "\"side\":\"" << side_to_cstr(off.side) << "\","
            << "\"dx_px\":" << off.dx_px << ","
            << "\"dx_norm\":" << off.dx_norm << ","
            << "\"confidence\":" << off.confidence
            << "}\n";

  // 退出码契约：0 表示得到有效目标；2 表示业务上无有效目标（missing/low）；其余非零为异常
  return off.valid ? 0 : 2;
}
