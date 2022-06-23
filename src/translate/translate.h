#ifndef __TRANSLATE_H
#define __TRANSLATE_H

#include <string>
#if defined(MAGFY_WINDOWS)
#include <windows.h>
#endif

#if defined(MAGFY_WINDOWS)
using Modifiers = UINT;
using Key = UINT;
using Button = unsigned int;
#else
using Modifiers = unsigned int;
using Key = unsigned int;
using Button = unsigned int;
#endif

enum class ShortcutState {
    NONE,
    VALID,
};

class Shortcut {
  public:
    ShortcutState state;
    Modifiers modifiers;

    Shortcut(Modifiers modifiers = 0);

    void set_state(ShortcutState);
    void add_modifier(const std::string &str_modifier);
};

class KeyShortcut : public Shortcut {
  public:
    Key key;

    KeyShortcut(Modifiers modifiers = 0, Key key = 0);

    void add_key(const std::string &str_key);

    bool operator==(const KeyShortcut& other);
};

class ButtonShortcut : public Shortcut {
  public:
    Button button;

    ButtonShortcut(Modifiers modifiers = 0, Button button = 0);

    void add_button(const std::string &str_button);

    bool operator==(const ButtonShortcut& other);
};

KeyShortcut translate_into_key(const std::string &sequence);

ButtonShortcut translate_into_button(const std::string &sequence);

#endif // __TRANSLATE_H