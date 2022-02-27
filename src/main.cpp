#include <spdlog/spdlog.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/yaml.h>

#if defined(MAGFY_WINDOWS)
#include <spdlog/sinks/basic_file_sink.h>
#include <windows.h>
#endif

#include "Config.h"
#include "core.h"

#if defined(MAGFY_WINDOWS)
std::shared_ptr<spdlog::logger> logger =
    spdlog::basic_logger_mt("magfy", "log.txt");
#else
#endif

#if defined(MAGFY_WINDOWS)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
    // logger setting
    spdlog::flush_every(std::chrono::seconds(3));

    YAML::Node root = YAML::LoadFile(get_config_file());
    Config config;

    try {
        config = root.as<Config>();
    } catch (YAML::Exception ex) {
        logger->error("Could not parse the config file.");
        logger->error("Parse error => {}", ex.msg);
        logger->warn("Terminated abnormally.");
        return 1;
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
        logger->warn("Terminated abnormally.");
        return 1;
    }
}