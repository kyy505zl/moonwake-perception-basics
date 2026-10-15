// include/moonwake/buoy_offset.hpp
#pragma once

#include "moonwake/detection.hpp"
#include <optional>

namespace moonwake {

/**
 * @brief 根据图像宽度与一次浮标检测，计算浮标中心相对画面中心的水平偏移
 * 
 * @param det 可选浮标检测目标（std::nullopt 表示未检测到目标）
 * @param image_width 图像宽度（像素，必须 > 0）
 * @param conf_threshold 置信度门控阈值，低于该阈值视为无效目标（默认 0.35）
 * @param center_tolerance_norm 归一化居中容差，|dx_norm| <= 此值视为 Center（默认 0.08）
 * @return BuoyOffset 包含有效性 valid、方位 side、dx_px 与 dx_norm 的计算结果
 */
BuoyOffset compute_buoy_offset(
    const std::optional<Detection>& det,
    int image_width,
    double conf_threshold = 0.35,
    double center_tolerance_norm = 0.08);

}  // namespace moonwake
