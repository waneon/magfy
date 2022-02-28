#include <Shlobj.h>
#include <spdlog/spdlog.h>
#include <string>
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
Config *global_config = nullptr;

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

// wchar_t* to std::string
// https://wendys.tistory.com/84
DWORD convert_unicode_to_ansi_string(__out std::string &ansi,
                                     __in const wchar_t *unicode,
                                     __in const size_t unicode_size) {
    DWORD error = 0;
    do {
        if ((nullptr == unicode) || (0 == unicode_size)) {
            error = ERROR_INVALID_PARAMETER;
            break;
        }
        ansi.clear();
        //
        // getting required cch.
        //
        int required_cch = ::WideCharToMultiByte(CP_ACP, 0, unicode,
                                                 static_cast<int>(unicode_size),
                                                 nullptr, 0, nullptr, nullptr);
        if (0 == required_cch) {
            error = ::GetLastError();
            break;
        }
        //
        // allocate.
        //
        ansi.resize(required_cch);
        //
        // convert.
        //
        if (0 == ::WideCharToMultiByte(
                     CP_ACP, 0, unicode, static_cast<int>(unicode_size),
                     const_cast<char *>(ansi.c_str()),
                     static_cast<int>(ansi.size()), nullptr, nullptr)) {
            error = ::GetLastError();
            break;
        }
    } while (false);
    return error;
}

std::string get_config_file() {
#if defined(NDEBUG)
    wchar_t *path_wchar = nullptr;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path_wchar);

    std::string path;
    convert_unicode_to_ansi_string(path, path_wchar, wcslen(path_wchar));

    path += "/magfy";
    CreateDirectory(path.c_str(), NULL);

    return path + "/config.yaml";
#else
    return CONFIG_FILE;
#endif
}

std::string get_log_file() {
    wchar_t *path_wchar = nullptr;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path_wchar);

    std::string path;
    convert_unicode_to_ansi_string(path, path_wchar, wcslen(path_wchar));

    path += "/magfy";
    CreateDirectory(path.c_str(), NULL);

    return path + "/log.txt";
}

bool run(HINSTANCE hInstance, Config &config) {
    // check whether magfy application is already running
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, MAGFY_MUTEX_NAME);
    if (!hMutex) {
        hMutex = CreateMutex(0, 0, MAGFY_MUTEX_NAME);
    } else {
        logger->error("Magfy is already running.");
        return false;
    }

    // global config
    global_config = &config;

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
    } else { // mouse shortcut
        ButtonShortcut cur = {};
        cur.state = ShortcutState::VALID;
        cur.button = wParam;

        // cur extra
        auto hook_struct = (MSLLHOOKSTRUCT *)lParam;
        switch (wParam) {
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            if (GET_WHEEL_DELTA_WPARAM(hook_struct->mouseData) > 0) {
                cur.extra = 1;
            } else {
                cur.extra = -1;
            }
            break;
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            cur.extra = GET_XBUTTON_WPARAM(hook_struct->mouseData);
            break;
        default:
            cur.extra = 0;
            break;
        }

        // cur modifiers
        if (GetKeyState(VK_CONTROL) & 0x8000)
            cur.modifiers |= MOD_CONTROL;
        if (GetKeyState(VK_MENU) & 0x8000)
            cur.modifiers |= MOD_ALT;
        if (GetKeyState(VK_SHIFT) & 0x8000)
            cur.modifiers |= MOD_SHIFT;
        if (cur.modifiers == 0)
            cur.modifiers = MOD_NOREPEAT;

        // shortcut comparison
        if (cur == global_config->toggle_button) {
            magnifier->toggle();
            magnifier->update();
            return -1;
        } else if (cur == global_config->shrink_button) {
            magnifier->shrink();
            magnifier->update();
            return -1;
        } else if (cur == global_config->enlarge_button) {
            magnifier->enlarge();
            magnifier->update();
            return -1;
        } else if (cur == global_config->exit_button) {
            PostQuitMessage(0);
            return -1;
        }

        // button-up event hook
        switch (wParam) {
        case WM_LBUTTONUP:
            cur.button = WM_LBUTTONDOWN;
            break;
        case WM_MBUTTONUP:
            cur.button = WM_LBUTTONDOWN;
            break;
        case WM_RBUTTONUP:
            cur.button = WM_RBUTTONUP;
            break;
        case WM_XBUTTONUP:
            cur.button = WM_XBUTTONDOWN;
            break;
        }

        if (cur == global_config->toggle_button) {
            return -1;
        } else if (cur == global_config->shrink_button) {
            return -1;
        } else if (cur == global_config->enlarge_button) {
            return -1;
        } else if (cur == global_config->exit_button) {
            return -1;
        }
    }

    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
}