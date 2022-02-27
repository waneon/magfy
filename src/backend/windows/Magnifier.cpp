#include <algorithm>
#include <magnification.h>
#include <spdlog/spdlog.h>
#include <windows.h>

#include "./Magnifier.h"

extern std::shared_ptr<spdlog::logger> logger;

namespace windows {
const float Magnifier::dead_zone_factor = 0.1;

Magnifier::Magnifier(const Config &config) : ::Magnifier(config) {
    is_tracking = true;

    MagInitialize();
}

Magnifier::~Magnifier() { MagUninitialize(); }

int Magnifier::transform_x(int x) const {
    int width = GetSystemMetrics(SM_CXSCREEN);

    float mul_x =
        width * (1 - 1 / cur_mag_factor) / ((1 - 2 * dead_zone_factor) * width);
    float sub_x = mul_x * dead_zone_factor * width;
    int max_x = static_cast<int>(width * (1 - 1 / cur_mag_factor));

    return std::clamp(static_cast<int>(x * mul_x - sub_x), 0, max_x);
}

int Magnifier::transform_y(int y) const {
    int height = GetSystemMetrics(SM_CYSCREEN);

    float mul_y = height * (1 - 1 / cur_mag_factor) /
                  ((1 - 2 * dead_zone_factor) * height);
    float sub_y = mul_y * dead_zone_factor * height;
    int max_y = static_cast<int>(height * (1 - 1 / cur_mag_factor));

    return std::clamp(static_cast<int>(y * mul_y - sub_y), 0, max_y);
}

void Magnifier::magnify() {
    is_magnified = true;
    cur_mag_factor = mag_factor;
    logger->info("Set magnifier mag-factor to {}", cur_mag_factor);
}

void Magnifier::unmagnify() {
    is_magnified = false;
    cur_mag_factor = 1.0;
    logger->info("Set magnifier mag-factor to {}", cur_mag_factor);
}

void Magnifier::enlarge() {
    if (is_magnified) {
        cur_mag_factor += mag_enlarge_factor;
        logger->info("Set magnifier mag-factor to {}", cur_mag_factor);
    }
}

void Magnifier::shrink() {
    if (is_magnified && cur_mag_factor - mag_shrink_factor > 0) {
        cur_mag_factor -= mag_shrink_factor;
        logger->info("Set magnifier mag-factor to {}", cur_mag_factor);
    }
}

void Magnifier::update() const {
    if (is_magnified) {
        POINT p{};
        GetCursorPos(&p);

        int x = transform_x(p.x);
        int y = transform_y(p.y);
        MagSetFullscreenTransform(cur_mag_factor, x, y);

        // input handling
        RECT rcDest;
        rcDest.left = 0;
        rcDest.top = 0;
        rcDest.right = GetSystemMetrics(SM_CXSCREEN);
        rcDest.bottom = GetSystemMetrics(SM_CYSCREEN);

        RECT rcSource;
        rcSource.left = x;
        rcSource.top = y;
        rcSource.right = rcSource.left + (int)(rcDest.right / cur_mag_factor);
        rcSource.bottom = rcSource.top + (int)(rcDest.bottom / cur_mag_factor);

        MagSetInputTransform(TRUE, &rcSource, &rcDest);

    } else {
        MagSetFullscreenTransform(1.0, 0, 0);
    }
}
} // namespace windows