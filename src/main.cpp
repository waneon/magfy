#include "Config.h"
#include "core.h"
#include <yaml-cpp/yaml.h>
#if defined(MAGFY_WINDOWS)
#include <windows.h>
#endif

// global variables
#if defined(MAGFY_WINDOWS)
HINSTANCE g_hInstance = NULL;
#endif

#if defined(MAGFY_WINDOWS)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
#if defined(MAGFY_WINDOWS)
    g_hInstance = hInstance;
#endif

    try {
        // parse config.yaml file
        YAML::Node root = YAML::LoadFile(get_config_file());
        Config config = root.as<Config>();

        // run magfy
        run(config);

        return 0;
    } catch (std::exception &ex) {
        error(ex.what());

        return 1;
    }
}