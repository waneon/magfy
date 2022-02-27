#include <algorithm>
#include <spdlog/spdlog.h>
#include <sstream>

#include "Config.h"
#include "translate.h"

extern std::shared_ptr<spdlog::logger> logger;

// parsing error
inline void invalid_value(std::string attribute) {
    logger->error("Invalid value of attribute \"{}\"", attribute);
}
inline void invalid_shortcut(std::string attribute) {
    logger->error("Invalid shortcut of attribute \"{}\"", attribute);
}

namespace YAML {
// parse yaml into Config
bool convert<Config>::decode(const Node &root, Config &rhs) {
    if (!root || !root.IsMap()) {
        invalid_value("root");
        return false;
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
            invalid_value("magnifier::backend");
            return false;
        }

        // mag-factor
        rhs.mag_factor = magnifier["mag-factor"].as<double>(1.5);

        // mag-enlarge-factor
        rhs.mag_enlarge_factor =
            magnifier["mag-enlarge-factor"].as<double>(0.5);

        // mag-shrink-factor
        rhs.mag_shrink_factor = magnifier["mag-shrink-factor"].as<double>(0.5);
    } else {
        invalid_value("magnifier");
        return false;
    }

    // keyboard-shortcut
    auto kshortcut = root["keyboard-shortcut"];
    if (kshortcut && kshortcut.IsMap()) {
        auto parse_key = [&](const std::string &name,
                             const std::string &default_sequence,
                             KeyShortcut &save) {
            auto sequence = kshortcut[name].as<std::string>(default_sequence);
            KeyShortcut shortcut = translate_into_key(sequence);
            if (shortcut.state == ShortcutState::INVALID) {
                invalid_shortcut(name);
                return false;
            }
            save = shortcut;
            return true;
        };

        // toggle
        if (!parse_key("toggle", "Alt+F1", rhs.toggle_key))
            return false;

        // shrink
        if (!parse_key("shrink", "", rhs.shrink_key))
            return false;

        // enlarge
        if (!parse_key("enlarge", "", rhs.enlarge_key))
            return false;

        // exit
        if (!parse_key("exit", "", rhs.exit_key))
            return false;

    } else {
        // toggle
        KeyShortcut toggle = translate_into_key("Alt+F1");
        if (toggle.state == ShortcutState::INVALID) {
            invalid_shortcut("keyboard-shortcut::toggle");
            return false;
        }
        rhs.toggle_key = toggle;

        // shrink
        rhs.shrink_key = {.state = ShortcutState::NONE};

        // enlarge
        rhs.enlarge_key = {.state = ShortcutState::NONE};

        // exit
        rhs.exit_key = {.state = ShortcutState::NONE};
    }

    // mouse-shortcut
    auto mshortcut = root["mouse-shortcut"];
    if (mshortcut && mshortcut.IsMap()) {
        auto parse_button = [&](const std::string &name,
                                const std::string &default_sequence,
                                ButtonShortcut &save) {
            auto sequence = mshortcut[name].as<std::string>(default_sequence);
            ButtonShortcut shortcut = translate_into_button(sequence);
            if (shortcut.state == ShortcutState::INVALID) {
                invalid_shortcut(name);
                return false;
            }
            save = shortcut;
            return true;
        };

        // toggle
        if (!parse_button("toggle", "", rhs.toggle_button))
            return false;

        // shrink
        if (!parse_button("shrink", "", rhs.shrink_button))
            return false;

        // enlarge
        if (!parse_button("enlarge", "", rhs.enlarge_button))
            return false;

        // exit
        if (!parse_button("exit", "", rhs.exit_button))
            return false;
    } else {
        // toggle
        rhs.toggle_button = {.state = ShortcutState::NONE};

        // shrink
        rhs.shrink_button = {.state = ShortcutState::NONE};

        // enlarge
        rhs.enlarge_button = {.state = ShortcutState::NONE};

        // exit
        rhs.exit_button = {.state = ShortcutState::NONE};
    }

    return true;
}
} // namespace YAML