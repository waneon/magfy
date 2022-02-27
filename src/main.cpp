#include <fstream>
#include <spdlog/spdlog.h>
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/yaml.h>

#include "Config.h"
#include "core.h"

int main() {
    Config config;

    try {
        YAML::Node root = YAML::LoadFile(get_config_file());
        config = root.as<Config>();
    } catch (YAML::BadFile ex) {
        spdlog::error("Could not find the config file.");
        spdlog::error(
            "Config file must be placed in ~/.config/magfy/config.yaml");
        goto error;
    } catch (YAML::Exception ex) {
        spdlog::error("Could not parse the config file.");
        spdlog::error("Parse error => {}", ex.msg);
        goto error;
    }
    spdlog::info("Successfully loaded the config file.");

    if (run(config)) {
        spdlog::info("Terminated normally.");
        return 0;
    } else {
    error:
        spdlog::warn("Terminated abnormally.");
        return 1;
    }
}