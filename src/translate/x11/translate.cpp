#include <X11/X.h>
#include <X11/Xlib.h>
#include <sstream>
#include <vector>

#include "translate.h"

static Modifiers lookup_modifier(const std::string &);
static KeyShortcut::Key lookup_key(const std::string &);
static ButtonShortcut::Button lookup_button(const std::string &);

KeyShortcut translate_into_key(const std::string &sequence) {
    std::istringstream is{sequence};
    std::vector<std::string> token;
    std::string buffer;

    while (std::getline(is, buffer, '+')) {
        token.push_back(buffer);
    }

    KeyShortcut ret{
        .state = ShortcutState::INVALID,
        .modifiers = 0,
        .key = 0,
    };

    // if there's no token
    if (token.size() == 0) {
        ret.state = ShortcutState::NONE;
        return ret;
    }

    // modifiers check
    for (auto it = token.begin(); it < token.end() - 1; it++) {
        Modifiers m = lookup_modifier(*it);
        if (m == 0) {
            return ret;
        }
        ret.modifiers |= m;
    }
    // if there's no modifiers
    if (ret.modifiers == 0) {
        ret.modifiers = AnyModifier;
    }

    // key check
    KeyShortcut::Key k = lookup_key(*token.rbegin());
    if (k == 0) {
        return ret;
    }
    ret.key = k;

    ret.state = ShortcutState::VALID;
    return ret;
}

ButtonShortcut translate_into_button(const std::string &sequence) {
    std::istringstream is{sequence};
    std::vector<std::string> token;
    std::string buffer;

    while (std::getline(is, buffer, '+')) {
        token.push_back(buffer);
    }

    ButtonShortcut ret{
        .state = ShortcutState::INVALID,
        .modifiers = 0,
        .button = 0,
    };

    // if there's no token
    if (token.size() == 0) {
        ret.state = ShortcutState::NONE;
        return ret;
    }

    // modifiers check
    for (auto it = token.begin(); it < token.end() - 1; it++) {
        Modifiers m = lookup_modifier(*it);
        if (m == 0) {
            return ret;
        }
        ret.modifiers |= m;
    }
    // if there's no modifiers
    if (ret.modifiers == 0) {
        ret.modifiers = AnyModifier;
    }

    // key check
    ButtonShortcut::Button b = lookup_button(*token.rbegin());
    if (b == 0) {
        return ret;
    }
    ret.button = b;

    ret.state = ShortcutState::VALID;
    return ret;
}

Modifiers lookup_modifier(const std::string &string) {
    if (string == "Ctrl") {
        return ControlMask;
    } else if (string == "Alt") {
        return Mod1Mask;
    } else if (string == "Shift") {
        return ShiftMask;
    } else {
        return None;
    }
}

KeyShortcut::Key lookup_key(const std::string &string) {
    Display *dpy = XOpenDisplay(0);

    KeySym keysym = XStringToKeysym(string.c_str());
    if (keysym == NoSymbol) {
        XCloseDisplay(dpy);
        return None;
    }
    KeyShortcut::Key key = XKeysymToKeycode(dpy, keysym);

    XCloseDisplay(dpy);
    return key;
}

ButtonShortcut::Button lookup_button(const std::string &string) {
    if (string == "Left") {
        return 1;
    } else if (string == "Midde") {
        return 2;
    } else if (string == "Right") {
        return 3;
    } else if (string == "WheelUp") {
        return 4;
    } else if (string == "WheelDown") {
        return 5;
    } else if (string == "WheelLeft") {
        return 6;
    } else if (string == "WheelRight") {
        return 7;
    } else if (string == "Side1") {
        return 8;
    } else if (string == "Side2") {
        return 9;
    } else {
        return None;
    }
}