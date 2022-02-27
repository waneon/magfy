#include <spdlog/spdlog.h>

#include "./Magnifier.h"

namespace gnome {
Magnifier::Magnifier(const Config &config) : ::Magnifier(config) {
    // gnome magnifier on
    if (system(
            "gsettings set org.gnome.desktop.a11y.magnifier mag-factor 1.0") !=
        0) {
        spdlog::warn("gsetting terminated abnormally.");
    };
    if (system("gsettings set org.gnome.desktop.a11y.applications "
               "screen-magnifier-enabled true") != 0) {
        spdlog::warn("gsetting terminated abnormally.");
    };

    // gnome magnifier configure
    if (system("gsettings set org.gnome.desktop.a11y.magnifier caret-tracking "
               "none") != 0) {
        spdlog::warn("gsetting terminated abnormally.");
    };
    if (system("gsettings set org.gnome.desktop.a11y.magnifier focus-tracking "
               "none") != 0) {
        spdlog::warn("gsetting terminated abnormally.");
    };

    spdlog::info("Initialized magnifier.");
}

Magnifier::~Magnifier() {
    if (system("gsettings set org.gnome.desktop.a11y.applications "
               "screen-magnifier-enabled false") != 0) {
        spdlog::warn("gsetting terminated abnormally.");
    };
}

void Magnifier::update_mag_factor() {
    char command[100];
    sprintf(command,
            "gsettings set org.gnome.desktop.a11y.magnifier "
            "mag-factor %.2f",
            cur_mag_factor);
    if (system(command) != 0) {
        spdlog::warn("gsetting terminated abnormally.");
    };
}

void Magnifier::magnify() {
    is_magnified = true;
    cur_mag_factor = mag_factor;
    update_mag_factor();
    spdlog::info("Set magnifier mag-factor to {}", cur_mag_factor);
}

void Magnifier::unmagnify() {
    is_magnified = false;
    cur_mag_factor = 1.0;
    update_mag_factor();
    spdlog::info("Set magnifier mag-factor to {}", cur_mag_factor);
}

void Magnifier::enlarge() {
    if (is_magnified) {
        cur_mag_factor += mag_enlarge_factor;
        update_mag_factor();
        spdlog::info("Set magnifier mag-factor to {}", cur_mag_factor);
    }
}

void Magnifier::shrink() {
    if (is_magnified && cur_mag_factor - mag_shrink_factor > 0) {
        cur_mag_factor -= mag_shrink_factor;
        update_mag_factor();
        spdlog::info("Set magnifier mag-factor to {}", cur_mag_factor);
    }
}
} // namespace gnome