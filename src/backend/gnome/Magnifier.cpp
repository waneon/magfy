#include <spdlog/spdlog.h>

#include "./Magnifier.h"

extern std::shared_ptr<spdlog::logger> logger;

inline static void run_gsettings(const char *command) {
    if (system(command) != 0) {
        logger->error("gsettings terminated abnormally.");
        throw std::exception{};
    }
}

namespace gnome {
Magnifier::Magnifier(const Config &config) : ::Magnifier(config) {
    // gnome magnifier on
    run_gsettings(
        "gsettings set org.gnome.desktop.a11y.magnifier mag-factor 1.0");
    run_gsettings("gsettings set org.gnome.desktop.a11y.applications "
                  "screen-magnifier-enabled true");

    // gnome magnifier configure
    run_gsettings(
        "gsettings set org.gnome.desktop.a11y.magnifier caret-tracking "
        "none");
    run_gsettings(
        "gsettings set org.gnome.desktop.a11y.magnifier focus-tracking "
        "none");

    logger->info("Initialized magnifier.");
}

Magnifier::~Magnifier() {
    try {
        run_gsettings("gsettings set org.gnome.desktop.a11y.applications "
                      "screen-magnifier-enabled false");
    } catch (std::exception ex) {
        logger->warn("gsettings terminated abnormally.");
    }
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
    if (is_magnified && cur_mag_factor - mag_shrink_factor >= 1.0) {
        cur_mag_factor -= mag_shrink_factor;
        logger->info("Set magnifier mag-factor to {}", cur_mag_factor);
    }
}

void Magnifier::update() const {
    char command[100];
    sprintf(command,
            "gsettings set org.gnome.desktop.a11y.magnifier "
            "mag-factor %.2f",
            cur_mag_factor);
    run_gsettings(command);
}
} // namespace gnome