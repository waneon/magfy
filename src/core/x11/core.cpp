#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdlib>
#include <cstring>
#include <pwd.h>
#include <signal.h>
#include <spdlog/spdlog.h>
#include <sys/time.h>
#include <unistd.h>

#include "core.h"
#include "magnifiers.h"
#include "translate.h"

extern std::shared_ptr<spdlog::logger> logger;

// cooldown static variable
bool is_cooldowned = true;

// timer callback
void cooldown(int _signum) {
    is_cooldowned = true;
}

// register keyboard shortcut
inline static void register_key(const KeyShortcut &, std::string_view,
                                Display *, Window);
// un-register keyboard shortcut
inline static void unregister_key(const KeyShortcut &, std::string_view,
                                  Display *, Window);
// register mouse shortcut
inline static void register_button(const ButtonShortcut &, std::string_view,
                                   Display *, Window);
// un-register mouse shortcut
inline static void unregister_button(const ButtonShortcut &, std::string_view,
                                     Display *, Window);

// x11 error handler
static int x11_error_handler(Display *, XErrorEvent *e);

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

    // set error handler
    XSetErrorHandler(x11_error_handler);

    // Magnifier
    Magnifier *magnifier = nullptr;
    switch (config.backend) {
    case Backend::GNOME:
        magnifier = new gnome::Magnifier{config};
        break;
    default:
        logger->error("Could not load magnifier.");
        throw std::exception{};
    }

    // register shortcuts
    register_key(config.toggle_key, "toggle", dpy, root);
    register_key(config.shrink_key, "shrink", dpy, root);
    register_key(config.enlarge_key, "enlarge", dpy, root);
    register_key(config.exit_key, "exit", dpy, root);

    register_button(config.toggle_button, "toggle", dpy, root);
    register_button(config.shrink_button, "shrink", dpy, root);
    register_button(config.enlarge_button, "enlarge", dpy, root);
    register_button(config.exit_button, "exit", dpy, root);

    // select events
    XSelectInput(dpy, root, KeyPressMask);
    XSelectInput(dpy, root, ButtonPressMask);
    while (true) {
        XNextEvent(dpy, &ev);

        switch (ev.type) {
        case KeyPress:
            if (is_cooldowned == false)
                break;

            if (ev.xkey.keycode == config.toggle_key.key) {
                magnifier->toggle();
                goto update;
            } else if (ev.xkey.keycode == config.shrink_key.key) {
                magnifier->shrink();
                goto update;
            } else if (ev.xkey.keycode == config.enlarge_key.key) {
                magnifier->enlarge();
                goto update;
            } else if (ev.xkey.keycode == config.exit_key.key) {
                goto out;
            }
            break;
        case ButtonPress:
            if (is_cooldowned == false)
                break;

            if (ev.xbutton.button == config.toggle_button.button) {
                magnifier->toggle();
                goto update;
            } else if (ev.xbutton.button == config.shrink_button.button) {
                magnifier->shrink();
                goto update;
            } else if (ev.xbutton.button == config.enlarge_button.button) {
                magnifier->enlarge();
                goto update;
            } else if (ev.xbutton.button == config.exit_button.button) {
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
    unregister_key(config.toggle_key, "toggle", dpy, root);
    unregister_key(config.shrink_key, "shrink", dpy, root);
    unregister_key(config.enlarge_key, "enlarge", dpy, root);
    unregister_key(config.exit_key, "exit", dpy, root);

    unregister_button(config.toggle_button, "toggle", dpy, root);
    unregister_button(config.shrink_button, "shrink", dpy, root);
    unregister_button(config.enlarge_button, "enlarge", dpy, root);
    unregister_button(config.exit_button, "exit", dpy, root);
}

void register_key(const KeyShortcut &shortcut, std::string_view name,
                  Display *dpy, Window root) {
    if (shortcut.state != ShortcutState::NONE) {
        XGrabKey(dpy, shortcut.key, shortcut.modifiers, root, False,
                 GrabModeAsync, GrabModeAsync);
    }
}

void unregister_key(const KeyShortcut &shortcut, std::string_view name,
                    Display *dpy, Window root) {
    if (shortcut.state != ShortcutState::NONE) {
        XUngrabKey(dpy, shortcut.key, shortcut.modifiers, root);
    }
}

void register_button(const ButtonShortcut &shortcut, std::string_view name,
                     Display *dpy, Window root) {
    if (shortcut.state != ShortcutState::NONE) {
        XGrabButton(dpy, shortcut.button, shortcut.modifiers, root, False,
                    ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);
    }
}

void unregister_button(const ButtonShortcut &shortcut, std::string_view name,
                       Display *dpy, Window root) {
    if (shortcut.state != ShortcutState::NONE) {
        XUngrabButton(dpy, shortcut.button, shortcut.modifiers, root);
    }
}

int x11_error_handler(Display *dpy, XErrorEvent *e) {
    char error_msg[256];

    XGetErrorText(dpy, e->error_code, error_msg, sizeof(error_msg));
    spdlog::error("X11 error occured: {}, serial number: {}", error_msg,
                  e->serial);

    return 0;
}