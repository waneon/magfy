#include <spdlog/spdlog.h>
#include <string_view>
#include <windows.h>

#include "core.h"
#include "magnifiers.h"
#include "translate.h"

extern std::shared_ptr<spdlog::logger> logger;

// global objects
HWND hWndMagfy = nullptr;

// constants
static const char *MAGFY_MUTEX_NAME = "Magfy_Mutex.0";
static const char *MAGFY_WINDOW_CLASS_NAME = "Magfy_Window_Class";
static const char *MAGFY_WINDOW_NAME = "Magfy_Window";

// forward declarations
void RegisterMagfyWindowClass(HINSTANCE);
void CreateMagfyWindow(HINSTANCE);
LRESULT CALLBACK MagfyWndProc(HWND hWnd, UINT message, WPARAM wParam,
                              LPARAM lParam);

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

    // create magfy window
    RegisterMagfyWindowClass(hInstance);
    CreateMagfyWindow(hInstance);

    // event loop
    // MSG msg = {};
    // while (GetMessage(&msg, hWndMagfy, 0, 0)) {
    //     TranslateMessage(&msg);
    //     DispatchMessage(&msg);
    // }

    return true;
}

LRESULT CALLBACK MagfyWndProc(HWND hWnd, UINT message, WPARAM wParam,
                              LPARAM lParam) {
    return 0;
}

void RegisterMagfyWindowClass(HINSTANCE hInstance) {
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = MagfyWndProc;
    wcex.hInstance = hInstance;
    wcex.lpszClassName = MAGFY_WINDOW_CLASS_NAME;

    RegisterClassEx(&wcex);
}

void CreateMagfyWindow(HINSTANCE hInstance) {
    hWndMagfy = CreateWindowEx(0, MAGFY_WINDOW_CLASS_NAME, MAGFY_WINDOW_NAME, 0,
                               0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
}