#include "PWRSRCConfig.h"
#include "GMATConfig.h"
#include <fstream>
#include <stdexcept>

void PWRSRCConfig::set_baud_rate_sources(int baud_rate) {
    config["baud_rate_sources"] = std::to_string(baud_rate);
}

void PWRSRCConfig::set_timeout(int timeout_ms) {
    config["timeout"] = std::to_string(timeout_ms);
}

void PWRSRCConfig::set_max_current(double current) {
    config["max_current"] = std::to_string(current);
}

void PWRSRCConfig::set_max_voltage(double voltage) {
    config["max_voltage"] = std::to_string(voltage);
}

void PWRSRCConfig::saveConfigToFile(const std::string& filename) {
    std::ofstream config_file(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_PWRSRC");
    if (config_file.is_open()) {
        for (const auto& pair : config) {
            config_file << pair.first << "=" << pair.second << "\n";
        }
        config_file.close();
    } else {
        throw std::runtime_error("Unable to open file for writing: " + filename);
    }
}

void PWRSRCConfig::loadConfigFromFile(const std::string& filename) {
    std::ifstream config_file(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_PWRSRC");
    if (config_file.is_open()) {
        std::string line;
        while (std::getline(config_file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                config[key] = value;
            }
        }
        config_file.close();
    } else {
        throw std::runtime_error("Unable to open file for reading: " + filename);
    }
}