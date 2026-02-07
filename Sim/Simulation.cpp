#include "Simulation.h"
#include <fstream>
#include <stdexcept>
#include "GMATConfig.h"
#include <string>

void Simulation::set_cal_mx(double value) {
    config["cal_mx"] = value;
}
void Simulation::set_cal_cx(double value) {
    config["cal_cx"] = value;
}
void Simulation::set_cal_my(double value) {
    config["cal_my"] = value;
}
void Simulation::set_cal_cy(double value) {
    config["cal_cy"] = value;
}
void Simulation::set_cal_mz(double value) {
    config["cal_mz"] = value;
}
void Simulation::set_cal_cz(double value) {
    config["cal_cz"] = value;
}
void Simulation::set_speed_multiplier(double value) {
    config["speed_multiplier"] = value;
}
void Simulation::set_simulation_dt(double value) {
    config["simulation_dt"] = value;
}
void Simulation::set_control_kp(double value) {
    config["control_kp"] = value;
}
void Simulation::set_control_ki(double value) {
    config["control_ki"] = value;
}

void Simulation::saveConfigToFile(const std::string& filename) {
    std::ofstream config_file(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_Simulation");
    if (config_file.is_open()) {
        for (const auto& pair : config) {
            config_file << pair.first << "=" << pair.second << "\n";
        }
        config_file.close();
    } else {
        throw std::runtime_error("Unable to open file for writing: " + filename);
    }
}

void Simulation::loadConfigFromFile(const std::string& filename) {
    std::ifstream config_file(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_Simulation");
    if (config_file.is_open()) {
        std::string line;
        while (std::getline(config_file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                double value = std::stod(line.substr(pos + 1));
                config[key] = value;
            }
        }
        config_file.close();
    } else {
        throw std::runtime_error("Unable to open file for reading: " + filename);
    }
}