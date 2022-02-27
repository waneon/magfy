#include <spdlog/spdlog.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/yaml.h>

#include "Config.h"
#include "core.h"

int main() {
    YAML::Node root = YAML::LoadFile(get_config_file());
    Config config;

    try {
        config = root.as<Config>();
    } catch (YAML::Exception ex) {
        spdlog::error("Could not parse the config file.");
        spdlog::error("Parse error => {}", ex.msg);
        spdlog::warn("Terminated abnormally.");
        return 1;
    }
    spdlog::info("Successfully loaded the config file.");

    if (run(config)) {
        spdlog::info("Terminated normally.");
        return 0;
    } else {
        spdlog::warn("Terminated abnormally.");
        return 1;
    }
}