#ifndef __TRANSLATE_H
#define __TRANSLATE_H

#include <X11/Xlib.h>
#include <string>

using Modifiers = unsigned int;

enum class ShortcutState {
    NONE,
    INVALID,
    VALID,
};

struct KeyShortcut {
    using Key = int;

    ShortcutState state;
    Modifiers modifiers;
    Key key;
};

struct ButtonShortcut {
    using Button = unsigned int;

    ShortcutState state;
    Modifiers modifiers;
    Button button;
};

KeyShortcut translate_into_key(const std::string &);

ButtonShortcut translate_into_button(const std::string &);

#endif // __TRANSLATE_H