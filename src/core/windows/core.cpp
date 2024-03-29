#include "core.h"
#include "Config.h"
#include "magnifiers.h"
#include "translate.h"
#include "windows/Magnifier.h"
#include <Shlobj.h>
#include <string>
#include <string_view>
#include <windows.h>

static const char *MAGFY_MUTEX_NAME = "Magfy_Mutex.0";
const static char *SHORTCUT_NAME[4] = {"toggle", "shrink", "enlarge", "exit"};

// global objects
Magnifier *magnifier = nullptr;
Config *global_config = nullptr;

extern HINSTANCE g_hInstance;

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
    return "./config.yaml";
#else
    return CONFIG_FILE;
#endif
}

void run(const Config &config) {
    // dpi-aware setting
    SetProcessDPIAware();

    // check whether magfy application is already running
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, MAGFY_MUTEX_NAME);
    if (!hMutex) {
        hMutex = CreateMutex(0, 0, MAGFY_MUTEX_NAME);
    } else {
        throw StringException{"magfy is already running"};
    }

    // global config
    global_config = const_cast<Config *>(&config);

    // Magnifier
    switch (config.backend) {
    case Backend::WINDOWS:
        magnifier = new windows::Magnifier{config};
        break;
    default:
        throw StringException{"unknown backend"};
    }

    // register shortcuts
    for (int i = 0; i < 4; i++) {
        auto &kshortcut = config.key_shortcut[i];
        if (kshortcut.state != ShortcutState::NONE) {
            if (RegisterHotKey(NULL, i, kshortcut.modifiers, kshortcut.key) ==
                false) {
                throw StringException{"cannot bind shortcut"};
            }
        }
    }

    // mouse hook
    SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, g_hInstance, NULL);

    // event loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        switch (msg.message) {
        case WM_HOTKEY:
            switch (msg.wParam) {
            case 0:
                magnifier->toggle();
                magnifier->update();
                break;
            case 1:
                magnifier->shrink();
                magnifier->update();
                break;
            case 2:
                magnifier->enlarge();
                magnifier->update();
                break;
            case 3:
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
    for (int i = 0; i < 4; i++) {
        auto &kshortcut = config.key_shortcut[i];
        if (kshortcut.state != ShortcutState::NONE) {
            UnregisterHotKey(NULL, i);
        }
    }
}

static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < HC_ACTION)
        return CallNextHookEx(NULL, nCode, wParam, lParam);

    // mouse move
    if (wParam == WM_MOUSEMOVE) {
        magnifier->update();
    } else { // mouse shortcut
        // cur extra
        int extra = 0;
        auto hook_struct = (MSLLHOOKSTRUCT *)lParam;
        switch (wParam) {
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            if (GET_WHEEL_DELTA_WPARAM(hook_struct->mouseData) > 0) {
                extra = 1;
            } else {
                extra = -1;
            }
            break;
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            extra = GET_XBUTTON_WPARAM(hook_struct->mouseData);
            break;
        default:
            extra = 0;
            break;
        }

        // cur modifiers
        Modifiers modifiers = 0;
        if (GetKeyState(VK_CONTROL) & 0x8000)
            modifiers |= MOD_CONTROL;
        if (GetKeyState(VK_MENU) & 0x8000)
            modifiers |= MOD_ALT;
        if (GetKeyState(VK_SHIFT) & 0x8000)
            modifiers |= MOD_SHIFT;
        if (modifiers == 0)
            modifiers = MOD_NOREPEAT;

        ButtonShortcut cur = {modifiers, static_cast<Button>(wParam), extra};

        // shortcut comparison
        if (cur == global_config->button_shortcut[0]) {
            magnifier->toggle();
            magnifier->update();
            return -1;
        } else if (cur == global_config->button_shortcut[1]) {
            magnifier->shrink();
            magnifier->update();
            return -1;
        } else if (cur == global_config->button_shortcut[2]) {
            magnifier->enlarge();
            magnifier->update();
            return -1;
        } else if (cur == global_config->button_shortcut[3]) {
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
            cur.button = WM_RBUTTONDOWN;
            break;
        case WM_XBUTTONUP:
            cur.button = WM_XBUTTONDOWN;
            break;
        }

        if (cur == global_config->button_shortcut[0]) {
            return -1;
        } else if (cur == global_config->button_shortcut[1]) {
            return -1;
        } else if (cur == global_config->button_shortcut[2]) {
            return -1;
        } else if (cur == global_config->button_shortcut[3]) {
            return -1;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void error(std::string error_message) {
    MessageBox(NULL, error_message.c_str(), NULL, MB_OK);
}