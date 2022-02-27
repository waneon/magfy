#include <spdlog/spdlog.h>
#include <string_view>
#include <windows.h>

#include "core.h"
#include "magnifiers.h"
#include "translate.h"

static const char* MAGFY_MUTEX_NAME = "Magfy_Mutex.0";

std::string get_config_file() {
#if defined(NDEBUG)
#else
    return CONFIG_FILE;
#endif
}

bool run(const Config &config) {
    // check whether magfy application is already running
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, MAGFY_MUTEX_NAME);
    if (!hMutex) {
        hMutex = CreateMutex(0, 0, MAGFY_MUTEX_NAME);
    } else {
        MessageBox(NULL, "Magfy is already running!", NULL, MB_OK);

        return false;
    }

    return true;
}