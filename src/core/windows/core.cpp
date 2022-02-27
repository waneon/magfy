#include <spdlog/spdlog.h>
#include <string_view>
#include <windows.h>

#include "core.h"
#include "magnifiers.h"
#include "translate.h"
#include "windows/Magnifier.h"

extern std::shared_ptr<spdlog::logger> logger;

// global objects
Magnifier *magnifier = nullptr;
HHOOK mouse_hook = nullptr;

// constants
static const char *MAGFY_MUTEX_NAME = "Magfy_Mutex.0";
enum HotkeyID {
    TOGGLE,
    SHRINK,
    ENLARGE,
    EXIT,
};

// register keyboard shortcut
static bool register_key(const KeyShortcut &, std::string_view, int);
// un-register keyboard shortcut
static bool unregister_key(const KeyShortcut &, std::string_view, int);

// mouse hook proc
static LRESULT CALLBACK MouseHookProc(int, WPARAM, LPARAM);

std::string get_config_file() {
#if defined(NDEBUG)
#else
    return CONFIG_FILE;
#endif
}

bool run(HINSTANCE hInstance, const Config &config) {
    // check whether magfy application is already running
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, MAGFY_MUTEX_NAME);
    if (!hMutex) {
        hMutex = CreateMutex(0, 0, MAGFY_MUTEX_NAME);
    } else {
        logger->error("Magfy is already running.");
        return false;
    }

    // Magnifier
    switch (config.backend) {
    case Backend::WINDOWS:
        magnifier = new windows::Magnifier{config};
        break;
    default:
        logger->error("Could not load magnifier.");
        return false;
    }

    // register shortcuts
    if (!register_key(config.toggle_key, "toggle", HotkeyID::TOGGLE))
        return false;
    if (!register_key(config.shrink_key, "shrink", HotkeyID::SHRINK))
        return false;
    if (!register_key(config.enlarge_key, "enlarge", HotkeyID::ENLARGE))
        return false;
    if (!register_key(config.exit_key, "exit", HotkeyID::EXIT))
        return false;

    // mouse hook
    mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hInstance, NULL);

    // event loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        switch (msg.message) {
        case WM_HOTKEY:
            switch (msg.wParam) {
            case HotkeyID::TOGGLE:
                magnifier->toggle();
                magnifier->update();
                break;
            case HotkeyID::SHRINK:
                magnifier->shrink();
                magnifier->update();
                break;
            case HotkeyID::ENLARGE:
                magnifier->enlarge();
                magnifier->update();
                break;
            case HotkeyID::EXIT:
                PostQuitMessage(0);
                break;
            default:
                logger->error("Unknown hotkey id.");
                PostQuitMessage(0);
                break;
            }
            break;
        default:
            break;
        }
    }

    // free
    delete magnifier;

    // un-register shortcuts
    if (!unregister_key(config.toggle_key, "toggle", HotkeyID::TOGGLE))
        return false;
    if (!unregister_key(config.shrink_key, "shrink", HotkeyID::SHRINK))
        return false;
    if (!unregister_key(config.enlarge_key, "enlarge", HotkeyID::ENLARGE))
        return false;
    if (!unregister_key(config.exit_key, "exit", HotkeyID::EXIT))
        return false;

    return true;
}

bool register_key(const KeyShortcut &shortcut, std::string_view name, int id) {
    if (shortcut.state != ShortcutState::NONE) {
        if (RegisterHotKey(NULL, id, shortcut.modifiers, shortcut.key) ==
            false) {
            logger->error("Could not register key shortcut: {}", name);
            return false;
        }
    }
    return true;
}

bool unregister_key(const KeyShortcut &shortcut, std::string_view name,
                    int id) {
    if (shortcut.state != ShortcutState::NONE) {
        if (UnregisterHotKey(NULL, id) == false) {
            logger->error("Could not un-register key shortcut: {}", name);
            return false;
        }
    }
    return true;
}

static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // mouse move
    if (wParam == WM_MOUSEMOVE) {
        magnifier->update();
    }

    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}