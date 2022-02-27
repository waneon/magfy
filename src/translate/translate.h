#ifndef __TRANSLATE_H
#define __TRANSLATE_H

#include <string>

#if defined(MAGFY_WINDOWS)
#include <windows.h>
#endif

#if defined(MAGFY_WINDOWS)
using Modifiers = UINT;
#else
using Modifiers = unsigned int;
#endif

enum class ShortcutState {
    NONE,
    INVALID,
    VALID,
};

struct KeyShortcut {
#if defined(MAGFY_WINDOWS)
    using Key = UINT;
#else
    using Key = int;
#endif

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