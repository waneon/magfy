#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <endian.h>
#include <spdlog/spdlog.h>
#include <string_view>

#include "core.h"
#include "magnifiers.h"
#include "translate.h"

// register keyboard shortcut
static bool register_key(const KeyShortcut &, std::string_view, Display *,
                         Window);
// un-register keyboard shortcut
static bool unregister_key(const KeyShortcut &, std::string_view, Display *,
                           Window);
// register mouse shortcut
static bool register_button(const ButtonShortcut &, std::string_view, Display *,
                            Window);
// un-register mouse shortcut
static bool unregister_button(const ButtonShortcut &, std::string_view,
                              Display *, Window);

std::string get_config_file() {
#if defined(NDEBUG)
#else
    return CONFIG_FILE;
#endif
}

bool run(const Config &config) {
    static Display *dpy = XOpenDisplay(0);
    static Window root = XDefaultRootWindow(dpy);
    XEvent ev;

    // Magnifier
    Magnifier *magnifier = nullptr;
    switch (config.backend) {
    case Backend::GNOME:
        magnifier = new gnome::Magnifier{config};
        break;
    default:
        spdlog::error("Could not load magnifier.");
        return false;
    }

    // register shortcuts
    if (!register_key(config.toggle_key, "toggle", dpy, root))
        return false;
    if (!register_key(config.shrink_key, "shrink", dpy, root))
        return false;
    if (!register_key(config.enlarge_key, "enlarge", dpy, root))
        return false;
    if (!register_key(config.exit_key, "exit", dpy, root))
        return false;

    if (!register_button(config.toggle_button, "toggle", dpy, root))
        return false;
    if (!register_button(config.shrink_button, "shrink", dpy, root))
        return false;
    if (!register_button(config.enlarge_button, "enlarge", dpy, root))
        return false;
    if (!register_button(config.exit_button, "exit", dpy, root))
        return false;

    XSelectInput(dpy, root, KeyPressMask);
    XSelectInput(dpy, root, ButtonPressMask);
    while (true) {
        XNextEvent(dpy, &ev);

        switch (ev.type) {
        case KeyPress:
            if (ev.xkey.keycode == config.toggle_key.key) {
                magnifier->toggle();
            } else if (ev.xkey.keycode == config.shrink_key.key) {
                magnifier->shrink();
            } else if (ev.xkey.keycode == config.enlarge_key.key) {
                magnifier->enlarge();
            } else if (ev.xkey.keycode == config.exit_key.key) {
                goto out;
            }
        case ButtonPress:
            if (ev.xbutton.button == config.toggle_button.button) {
                magnifier->toggle();
            } else if (ev.xbutton.button == config.shrink_button.button) {
                magnifier->shrink();
            } else if (ev.xbutton.button == config.enlarge_button.button) {
                magnifier->enlarge();
            } else if (ev.xbutton.button == config.exit_button.button) {
                goto out;
            }
        }
    }
out:

    // free
    delete magnifier;
    XCloseDisplay(dpy);

    // un-register shortcuts
    if (!unregister_key(config.toggle_key, "toggle", dpy, root))
        return false;
    if (!unregister_key(config.enlarge_key, "enlarge", dpy, root))
        return false;
    if (!unregister_key(config.shrink_key, "shrink", dpy, root))
        return false;
    if (!unregister_key(config.exit_key, "exit", dpy, root))
        return false;

    if (!unregister_button(config.toggle_button, "toggle", dpy, root))
        return false;
    if (!unregister_button(config.enlarge_button, "enlarge", dpy, root))
        return false;
    if (!unregister_button(config.shrink_button, "shrink", dpy, root))
        return false;
    if (!unregister_button(config.exit_button, "exit", dpy, root))
        return false;

    return true;
}

bool register_key(const KeyShortcut &shortcut, std::string_view name,
                  Display *dpy, Window root) {
    if (shortcut.state != ShortcutState::NONE) {
        if (!XGrabKey(dpy, shortcut.key, shortcut.modifiers, root, False,
                      GrabModeAsync, GrabModeAsync)) {
            spdlog::error("Could not register {} key shortcut.", name);
            return false;
        } else {
            spdlog::info("Successfully registered {} key shortcut.", name);
        };
    }
    return true;
}

static bool unregister_key(const KeyShortcut &shortcut, std::string_view name,
                           Display *dpy, Window root) {
    if (shortcut.state != ShortcutState::NONE) {
        if (!XUngrabKey(dpy, shortcut.key, shortcut.modifiers, root)) {
            spdlog::error("Could not un-register {} key shortcut.", name);
            return false;
        } else {
            spdlog::info("Successfully un-registered {} key shortcut.", name);
        };
    }
    return true;
}

bool register_button(const ButtonShortcut &shortcut, std::string_view name,
                     Display *dpy, Window root) {
    if (shortcut.state != ShortcutState::NONE) {
        if (!XGrabButton(dpy, shortcut.button, shortcut.modifiers, root, False,
                         ButtonPressMask, GrabModeAsync, GrabModeAsync, None,
                         None)) {
            spdlog::error("Could not register {} button shortcut.", name);
            return false;
        } else {
            spdlog::info("Successfully registered {} button shortcut.", name);
        };
    }
    return true;
}

static bool unregister_button(const ButtonShortcut &shortcut,
                              std::string_view name, Display *dpy,
                              Window root) {
    if (shortcut.state != ShortcutState::NONE) {
        if (!XUngrabButton(dpy, shortcut.button, shortcut.modifiers, root)) {
            spdlog::error("Could not un-register {} button shortcut.", name);
            return false;
        } else {
            spdlog::info("Successfully un-registered {} button shortcut.",
                         name);
        };
    }
    return true;
}