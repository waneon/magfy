#include "translate.h"
#include <sstream>
#include <vector>
#if defined(MAGFY_WINDOWS)
#include <windows.h>
#else
#include <X11/X.h>
#include <X11/Xlib.h>
#endif

#if defined(MAGFY_WINDOWS)
const static UINT ANY_MODIFIERS = MOD_NOREPEAT;
#else
const static Modifiers ANY_MODIFIERS = AnyModifier;
#endif

#if defined(MAGFY_WINDOWS)
static int lookup_button_extra(const std::string &);
#endif

// shortcut class implements
Shortcut::Shortcut(Modifiers modifiers)
    : state(ShortcutState::NONE), modifiers(modifiers) {}

void Shortcut::set_state(ShortcutState state) { this->state = state; }

void Shortcut::add_modifier(const std::string &str_modifier) {
#if defined(MAGFY_WINDOWS)
    if (string == "Ctrl") {
        return MOD_CONTROL;
    } else if (string == "Alt") {
        return MOD_ALT;
    } else if (string == "Shift") {
        return MOD_SHIFT;
    } else {
        return 0;
    }
#else
    if (str_modifier == "Ctrl") {
        this->modifiers |= ControlMask;
    } else if (str_modifier == "Alt") {
        this->modifiers |= Mod1Mask;
    } else if (str_modifier == "Shift") {
        this->modifiers |= ShiftMask;
    } else {
        throw std::exception{};
    }
#endif
}

// keyshortcut class implements
KeyShortcut::KeyShortcut(Modifiers modifiers, Key key)
    : Shortcut(modifiers), key(key) {}

void KeyShortcut::add_key(const std::string &str_key) {
#if defined(MAGFY_WINDOWS)
    // A-Z
    if (string.size() == 1 && string[0] >= 'A' && string[0] <= 'Z') {
        return string[0];
    }

    // 0-9
    if (string.size() == 1 && string[0] >= '0' && string[0] <= '9') {
        return string[0];
    }

    // F1-F9
    if (string.size() == 2 && string[0] == 'F' && string[1] >= '1' &&
        string[1] <= '9') {
        return VK_F1 + (string[1] - '1');
    }
    // F10 - F12
    if (string == "F10") {
        return VK_F10;
    } else if (string == "F11") {
        return VK_F11;
    } else if (string == "F12") {
        return VK_F12;
    }

    return 0;
#else
    Display *dpy = XOpenDisplay(0);

    KeySym keysym = XStringToKeysym(str_key.c_str());
    if (keysym == NoSymbol) {
        XCloseDisplay(dpy);
        throw std::exception{};
    }
    Key key = XKeysymToKeycode(dpy, keysym);

    XCloseDisplay(dpy);

    this->key = key;
#endif
}

bool KeyShortcut::operator==(const KeyShortcut &other) {
    if (this->key == other.key && (this->modifiers == other.modifiers ||
                                   this->modifiers == ANY_MODIFIERS ||
                                   other.modifiers == ANY_MODIFIERS)) {
        return true;
    }
    return false;
}

// buttonshortcut class implements
ButtonShortcut::ButtonShortcut(Modifiers modifiers, Button button)
    : Shortcut(modifiers), button(button) {}

void ButtonShortcut::add_button(const std::string &str_button) {
#if defined(MAGFY_WINDOWS)
    if (string == "Left") {
        return WM_LBUTTONDOWN;
    } else if (string == "Midde") {
        return WM_MBUTTONDOWN;
    } else if (string == "Right") {
        return WM_RBUTTONDOWN;
    } else if (string == "WheelUp") {
        return WM_MOUSEWHEEL;
    } else if (string == "WheelDown") {
        return WM_MOUSEWHEEL;
    } else if (string == "WheelLeft") {
        return WM_MOUSEHWHEEL;
    } else if (string == "WheelRight") {
        return WM_MOUSEHWHEEL;
    } else if (string == "Side1") {
        return WM_XBUTTONDOWN;
    } else if (string == "Side2") {
        return WM_XBUTTONDOWN;
    } else {
        return 0;
    }
#else
    if (str_button == "Left") {
        this->button = 1;
    } else if (str_button == "Midde") {
        this->button = 2;
    } else if (str_button == "Right") {
        this->button = 3;
    } else if (str_button == "WheelUp") {
        this->button = 4;
    } else if (str_button == "WheelDown") {
        this->button = 5;
    } else if (str_button == "WheelLeft") {
        this->button = 6;
    } else if (str_button == "WheelRight") {
        this->button = 7;
    } else if (str_button == "Side1") {
        this->button = 8;
    } else if (str_button == "Side2") {
        this->button = 9;
    } else {
        throw std::exception{};
    }
#endif
}

bool ButtonShortcut::operator==(const ButtonShortcut &other) {
    if (this->button == other.button && (this->modifiers == other.modifiers ||
                                         this->modifiers == ANY_MODIFIERS ||
                                         other.modifiers == ANY_MODIFIERS)) {
        return true;
    }
    return false;
}

#if defined(MAGFY_WINDOWS)
int lookup_button_extra(const std::string &string) {
    if (string == "WheelUp") {
        return 1;
    } else if (string == "WheelDown") {
        return -1;
    } else if (string == "WheelLeft") {
        return -1;
    } else if (string == "WheelRight") {
        return 1;
    } else if (string == "Side1") {
        return 1;
    } else if (string == "Side2") {
        return 2;
    } else {
        return 0;
    }
}
#endif

KeyShortcut translate_into_key(const std::string &sequence) {
    std::istringstream is{sequence};
    std::vector<std::string> token;
    std::string buffer;
    KeyShortcut ret{};

    // tokenize
    while (std::getline(is, buffer, '+')) {
        token.push_back(buffer);
    }
    // if there's no token
    if (token.size() == 0) {
        ret.state = ShortcutState::NONE;
        return ret;
    }

    // modifiers check
    for (auto it = token.begin(); it < token.end() - 1; it++) {
        ret.add_modifier(*it);
    }
    // if there's no modifiers
    if (ret.modifiers == 0) {
        ret.modifiers = ANY_MODIFIERS;
    }

    // key check
    ret.add_key(*token.rbegin());

    ret.set_state(ShortcutState::VALID);
    return ret;
}

ButtonShortcut translate_into_button(const std::string &sequence) {
    std::istringstream is{sequence};
    std::vector<std::string> token;
    std::string buffer;
    ButtonShortcut ret{};

    // tokenize
    while (std::getline(is, buffer, '+')) {
        token.push_back(buffer);
    }
    // if there's no token
    if (token.size() == 0) {
        ret.state = ShortcutState::NONE;
        return ret;
    }

    // modifiers check
    for (auto it = token.begin(); it < token.end() - 1; it++) {
        ret.add_modifier(*it);
    }
    // if there's no modifiers
    if (ret.modifiers == 0) {
        ret.modifiers = ANY_MODIFIERS;
    }

    // button check
    ret.add_button(*token.rbegin());
#if defined(MAGFY_WINDOWS)
    ret.extra = lookup_button_extra(*token.rbegin());
#endif

    ret.set_state(ShortcutState::VALID);
    return ret;
}
