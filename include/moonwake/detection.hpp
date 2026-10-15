// include/moonwake/detection.hpp
#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace moonwake {

/**
 * @brief 目标分类强类型枚举
 */
enum class ObjectClass {
  Unknown = 0,
  GreenBuoy,
  RedBuoy,
  YellowBuoy,
  Obstacle,
  DockMarker
};

/**
 * @brief 单帧目标检测结果（语言层结构体，非 ROS 消息）
 */
struct Detection {
  int id{0};                             // 帧内临时编号，非全局唯一 ID
  ObjectClass class_id{ObjectClass::Unknown};
  double cx{0.0};                        // 框中心 x 坐标（像素）
  double cy{0.0};                        // 框中心 y 坐标（像素）
  double width{0.0};                     // 框宽（像素）
  double height{0.0};                    // 框高（像素）
  double confidence{0.0};                // 检测置信度 [0.0, 1.0]
  std::optional<std::int64_t> stamp_ns;  // 可选：观测时间戳（纳秒假时间或系统时钟）
};

/**
 * @brief 浮标相对于图像中心的方位枚举
 */
enum class BuoySide {
  Left,
  Center,
  Right,
  Invalid
};

/**
 * @brief 浮标水平偏移计算结果
 */
struct BuoyOffset {
  bool valid{false};                     // 是否计算出有效结果（无目标、低置信度时为 false）
  BuoySide side{BuoySide::Invalid};      // 目标所在方位
  double dx_px{0.0};                     // 水平像素偏移: cx - center_x（左负右正）
  double dx_norm{0.0};                   // 归一化偏移: dx_px / center_x，范围约在 [-1.0, 1.0]
  double confidence{0.0};                // 沿用检测框置信度
  std::optional<std::int64_t> stamp_ns;  // 沿用观测时间戳
};

}  // namespace moonwake
