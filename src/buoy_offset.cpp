// src/buoy_offset.cpp
#include "moonwake/buoy_offset.hpp"

#include <cmath>

namespace moonwake {

    BuoyOffset compute_buoy_offset(
        const std::optional<Detection>& det,
        int image_width,
        double conf_threshold,
        double center_tolerance_norm) {
        BuoyOffset out;

        // =========================================================================
        // Task 01: 实现检测结果与水平偏移计算
        //
        // 请遵循以下工程与业务规则完成实现：
        // 1. 防御性检查：
        //    - 当 image_width <= 0 时，输入非法，直接返回 out (valid=false)
        //    - 当 !det.has_value() 时，目标缺失，直接返回 out (valid=false)
        //
        // 2. 置信度门控：
        //    - 获取检测值 const Detection& d = *det;
        //    - 若 d.confidence < conf_threshold，说明置信度不足，直接返回 out (valid=false)
        //
        // 3. 几何偏移计算：
        //    - 图像横向中心: const double center_x = static_cast<double>(image_width) * 0.5;
        //    - 水平像素偏移: out.dx_px = d.cx - center_x; (左负右正)
        //    - 归一化偏移:   out.dx_norm = out.dx_px / center_x;
        //    - 填充元数据:   out.confidence = d.confidence; out.stamp_ns = d.stamp_ns;
        //    - 标记有效:     out.valid = true;
        //
        // 4. 方位判定 (BuoySide)：
        //    - 若 std::fabs(out.dx_norm) <= center_tolerance_norm，则判定为 BuoySide::Center;
        //    - 否则若 out.dx_norm < 0.0，则判定为 BuoySide::Left;
        //    - 否则判定为 BuoySide::Right;
        // =========================================================================

        if (image_width <= 0 || !det.has_value()) //我喜欢用or，但是AI说||更严谨
        {
            return out;
        }
        const Detection& d = *det;
        if (d.confidence < conf_threshold)
        {
                return out;
        }
        const double center_x = static_cast<double>(image_width) * 0.5;
        out.dx_px = d.cx - center_x;
        out.dx_norm = out.dx_px / center_x;
        out.confidence = d.confidence; out.stamp_ns = d.stamp_ns;
        out.valid = true;
        if (std::fabs(out.dx_norm) <= center_tolerance_norm)
        {
            out.side = BuoySide::Center;
        }
        else if(out.dx_norm < 0.0)
        {
            out.side = BuoySide::Left;
        }
        else
        {
            out.side = BuoySide::Right;
        }
        return out;
    }

}  // namespace moonwake
