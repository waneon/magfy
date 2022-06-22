#ifndef __CONFIG_H
#define __CONFIG_H

#include "translate.h"
#include <exception>
#include <string>
#include <yaml-cpp/yaml.h>

// supported backends
enum class Backend {
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
    // 0 = toggle   1 = shrink
    // 2 = enlarge  3 = exit
    KeyShortcut key_shortcut[4];

    // button-shortcut
    // 0 = toggle   1 = shrink
    // 2 = enlarge  3 = exit
    ButtonShortcut button_shortcut[4];
};

class StringException : public std::exception {
  private:
    std::string e;

  public:
    StringException(const std::string &e) : e(e) {}

    const char *what() const noexcept { return e.c_str(); }
};

namespace YAML {
template <> struct convert<Config> {
    static bool decode(const Node &node, Config &rhs);
};
} // namespace YAML

#endif // __CONFIG_H
