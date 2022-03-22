#include <fstream>
#include <spdlog/spdlog.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/yaml.h>

#if defined(MAGFY_WINDOWS)
#include <spdlog/sinks/basic_file_sink.h>
#include <windows.h>
#else
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

#include "Config.h"
#include "core.h"

// global logger object
std::shared_ptr<spdlog::logger> logger;

// global hInstance object for Windows
#if defined(MAGFY_WINDOWS)
HINSTANCE g_hInstance = NULL;
#endif

#if defined(MAGFY_WINDOWS)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
    // vendor-specific configure
#if defined(MAGFY_WINDOWS)
    logger = spdlog::basic_logger_mt("magfy", get_log_file());
    spdlog::flush_every(std::chrono::seconds(3));

    g_hInstance = hInstance;
#else
    logger = spdlog::stderr_color_mt("magfy");
#endif

    // parse config.yaml file
    Config config;
    try {
        YAML::Node root = YAML::LoadFile(get_config_file());
        config = root.as<Config>();
    } catch (YAML::BadFile ex) {
        logger->error("Config file must be placed in proper directory.");
        return 1;
    } catch (YAML::Exception ex) {
        logger->error("Parse error => {}", ex.msg);
        return 1;
    }
    logger->info("Successfully loaded the config file.");

    // run magfy
    try {
        run(config);
        logger->info("Terminated normally.");
        return 0;
    } catch (std::exception ex) {
        logger->error("Terminated abnormally.");
        return 1;
    }
}