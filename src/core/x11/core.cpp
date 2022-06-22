#include "core.h"
#include "Config.h"
#include "magnifiers.h"
#include "translate.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <pwd.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

// cooldown static variable
bool is_cooldowned = true;

void cooldown(int _signum) { is_cooldowned = true; }

std::string get_config_file() {
#if defined(NDEBUG)
    static const char *CONFIG_FILE = "/.config/magfy/config.yaml";

    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    return std::string{homedir} + std::string{CONFIG_FILE};
#else
    return CONFIG_FILE;
#endif
}

void run(const Config &config) {
    Display *dpy = XOpenDisplay(0);
    Window root = XDefaultRootWindow(dpy);
    XEvent ev;

    // set timer signal handler
    struct sigaction sa;

    memset(&sa, 0, sizeof sa);
    sa.sa_handler = cooldown;
    sigaction(SIGALRM, &sa, NULL);

    // init timer
    struct itimerval timer;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    timer.it_value.tv_sec = config.cooldown / 1000;
    timer.it_value.tv_usec = (config.cooldown % 1000) * 1000;

    // Magnifier
    Magnifier *magnifier = nullptr;
    switch (config.backend) {
    case Backend::GNOME:
        magnifier = new gnome::Magnifier{config};
        break;
    default:
        throw StringException{"Unknown backend"};
    }

    // register shortcuts
    for (int i = 0; i < 4; i++) {
        auto &kshortcut = config.key_shortcut[i];
        if (kshortcut.state != ShortcutState::NONE) {
            XGrabKey(dpy, kshortcut.key, kshortcut.modifiers, root, False,
                     GrabModeAsync, GrabModeAsync);
        }

        auto &bshortcut = config.button_shortcut[i];
        if (bshortcut.state != ShortcutState::NONE) {
            XGrabButton(dpy, bshortcut.button, bshortcut.modifiers, root, False,
                        ButtonPressMask, GrabModeAsync, GrabModeAsync, None,
                        None);
        }
    }

    // select events
    XSelectInput(dpy, root, KeyPressMask);
    XSelectInput(dpy, root, ButtonPressMask);
    while (true) {
        XNextEvent(dpy, &ev);

        KeyShortcut k{ev.xkey.state, ev.xkey.keycode};
        ButtonShortcut b{ev.xbutton.state, ev.xbutton.button};
        switch (ev.type) {
        case KeyPress:
            if (is_cooldowned == false)
                break;

            if (k == config.key_shortcut[0]) {
                magnifier->toggle();
                goto update;
            } else if (k == config.key_shortcut[1]) {
                magnifier->shrink();
                goto update;
            } else if (k == config.key_shortcut[2]) {
                magnifier->enlarge();
                goto update;
            } else if (k == config.key_shortcut[3]) {
                goto out;
            }
            break;
        case ButtonPress:
            if (is_cooldowned == false)
                break;

            if (b == config.button_shortcut[0]) {
                magnifier->toggle();
                goto update;
            } else if (b == config.button_shortcut[1]) {
                magnifier->shrink();
                goto update;
            } else if (b == config.button_shortcut[2]) {
                magnifier->enlarge();
                goto update;
            } else if (b == config.button_shortcut[3]) {
                goto out;
            }
            break;
        default:
            break;
        update:
            magnifier->update();
            is_cooldowned = false;
            setitimer(ITIMER_REAL, &timer, NULL);
            break;
        }
    }

out:
    // free
    delete magnifier;
    XCloseDisplay(dpy);

    // un-register shortcuts
    for (int i = 0; i < 4; i++) {
        auto &kshortcut = config.key_shortcut[i];
        if (kshortcut.state != ShortcutState::NONE) {
            XUngrabKey(dpy, kshortcut.key, kshortcut.modifiers, root);
        }

        auto &bshortcut = config.button_shortcut[i];
        if (bshortcut.state != ShortcutState::NONE) {
            XUngrabButton(dpy, bshortcut.button, bshortcut.modifiers, root);
        }
    }
}

void error(std::string error_message) {
    std::cout << error_message << std::endl;
}