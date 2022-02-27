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

#if defined(MAGFY_WINDOWS)
std::shared_ptr<spdlog::logger> logger =
    spdlog::basic_logger_mt("magfy", "log.txt");
#else
std::shared_ptr<spdlog::logger> logger = spdlog::stderr_color_mt("magfy");
#endif

#if defined(MAGFY_WINDOWS)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
    // logger setting
    spdlog::flush_every(std::chrono::seconds(3));

    // config
    Config config;

    try {
        YAML::Node root = YAML::LoadFile(get_config_file());
        config = root.as<Config>();
    } catch (YAML::BadFile ex) {
        logger->error("Could not find the config file.");
#if defined(MAGFY_WINDOWS)
        logger->error("Config file must be placed in the binary folder.");
#else
        logger->error(
            "Config file must be placed in ~/.config/magfy/config.yaml");
#endif
        goto error;
    } catch (YAML::Exception ex) {
        logger->error("Could not parse the config file.");
        logger->error("Parse error => {}", ex.msg);
        goto error;
    }
    logger->info("Successfully loaded the config file.");

#if defined(MAGFY_WINDOWS)
    if (run(hInstance, config)) {
#else
    if (run(config)) {
#endif
        logger->info("Terminated normally.");
        return 0;
    } else {
    error:
        logger->warn("Terminated abnormally.");
        return 1;
    }
}