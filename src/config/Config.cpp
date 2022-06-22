#include "Config.h"
#include "translate.h"
#include <algorithm>
#include <exception>
#include <sstream>

const static char *SHORTCUT_NAME[4] = {"toggle", "shrink", "enlarge", "exit"};

namespace YAML {
// parse yaml into Config
bool convert<Config>::decode(const Node &root, Config &rhs) {
    if (!root || !root.IsMap()) {
        throw StringException{"invalid root field"};
    }

    // magnifier
    auto magnifier = root["magnifier"];
    if (magnifier && magnifier.IsMap()) {
        // backend
        auto backend = magnifier["backend"].as<std::string>("none");
        if (backend == "gnome") {
            rhs.backend = Backend::GNOME;
        } else if (backend == "windows") {
            rhs.backend = Backend::WINDOWS;
        } else {
            throw StringException{"invalid magnifier::backend value"};
        }

        // mag-factor
        rhs.mag_factor = magnifier["mag-factor"].as<double>(1.5);

        // mag-enlarge-factor
        rhs.mag_enlarge_factor =
            magnifier["mag-enlarge-factor"].as<double>(0.5);

        // mag-shrink-factor
        rhs.mag_shrink_factor = magnifier["mag-shrink-factor"].as<double>(0.5);

        // cooldown
        rhs.cooldown = magnifier["cooldown"].as<int>(100);
    } else {
        throw StringException{"invalid magnifier field"};
    }

    // keyboard-shortcut
    auto kshortcut = root["keyboard-shortcut"];
    if (kshortcut && kshortcut.IsMap()) {
        for (int i = 0; i < 4; i++) {
            std::string seq = kshortcut[SHORTCUT_NAME[i]].as<std::string>("");
            try {
                rhs.key_shortcut[i] = translate_into_key(seq);
            } catch (std::exception ex) {
                std::string e = "invalid shortcut: keyboard-shortcut::";
                e += SHORTCUT_NAME[i];
                throw StringException{e};
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            rhs.key_shortcut[i] = translate_into_key("");
        }
    }

    // mouse-shortcut
    auto mshortcut = root["mouse-shortcut"];
    if (mshortcut && mshortcut.IsMap()) {
        for (int i = 0; i < 4; i++) {
            std::string seq = mshortcut[SHORTCUT_NAME[i]].as<std::string>("");
            try {
                rhs.button_shortcut[i] = translate_into_button(seq);
            } catch (std::exception ex) {
                std::string e = "invalid shortcut: mouse-shortcut::";
                e += SHORTCUT_NAME[i];
                throw StringException{e};
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            rhs.button_shortcut[i] = translate_into_button("");
        }
    }

    return true;
}
} // namespace YAML