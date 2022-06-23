#include "./Magnifier.h"
#include "Config.h"

inline static void run_gsettings(const char *command) {
    if (system(command) != 0) {
        throw StringException{"cannot run gsetting"};
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
}

Magnifier::~Magnifier() {
    run_gsettings("gsettings set org.gnome.desktop.a11y.applications "
                  "screen-magnifier-enabled false");
}

void Magnifier::magnify() {
    is_magnified = true;
    cur_mag_factor = mag_factor;
}

void Magnifier::unmagnify() {
    is_magnified = false;
    cur_mag_factor = 1.0;
}

void Magnifier::enlarge() {
    if (is_magnified) {
        cur_mag_factor += mag_enlarge_factor;
    }
}

void Magnifier::shrink() {
    if (is_magnified && cur_mag_factor - mag_shrink_factor >= 1.0) {
        cur_mag_factor -= mag_shrink_factor;
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