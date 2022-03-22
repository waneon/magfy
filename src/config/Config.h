#ifndef __CONFIG_H
#define __CONFIG_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "translate.h"

// supported backends
enum class Backend {
    NONE,
    WINDOWS,
    GNOME,
};

struct Config {
  public:
    // magnifier
    Backend backend;
    double mag_factor;
    double mag_enlarge_factor;
    double mag_shrink_factor;
    int cooldown;

    // keyboard-shortcut
    KeyShortcut toggle_key;
    KeyShortcut shrink_key;
    KeyShortcut enlarge_key;
    KeyShortcut exit_key;

    // button-shortcut
    ButtonShortcut toggle_button;
    ButtonShortcut shrink_button;
    ButtonShortcut enlarge_button;
    ButtonShortcut exit_button;
};

namespace YAML {
template <> struct convert<Config> {
    static bool decode(const Node &node, Config &rhs);
};
} // namespace YAML

#endif // __CONFIG_H
