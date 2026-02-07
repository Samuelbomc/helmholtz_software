#include "ArduinoConfig.h"
#include "GMATConfig.h"
#include <fstream>

void ArduinoConfig::set_serial_timeout(double timeout) {
    config["serial_timeout"] = std::to_string(timeout);
}

void ArduinoConfig::set_mag_baud_rate(int baud_rate) {
    config["mag_baud_rate"] = std::to_string(baud_rate);
}

void ArduinoConfig::set_relay_baud_rate(int baud_rate) {
    config["relay_baud_rate"] = std::to_string(baud_rate);
}

void ArduinoConfig::set_magnetometer_port(const std::string& port) {
    config["magnetometer_port"] = port;
}

void ArduinoConfig::set_relay_port(const std::string& port) {
    config["relay_port"] = port;
}

void ArduinoConfig::saveConfigToFile(const std::string& filename) {
    std::ofstream config_file(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_Arduino");
    if (config_file.is_open()) {
        for (const auto& pair : config) {
            config_file << pair.first << "=" << pair.second << "\n";
        }
        config_file.close();
    }
    else {
        throw std::runtime_error("Unable to open file for writing: " + filename);
    }
}

void ArduinoConfig::loadConfigFromFile(const std::string& filename) {
    std::ifstream config_file(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_Arduino");
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
    }
    else {
        throw std::runtime_error("Unable to open file for reading: " + filename);
    }
}
