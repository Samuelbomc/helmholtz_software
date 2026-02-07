#pragma once
#include <map>
#include <string>

class Simulation {
public:
    // Calibration constants
    void set_cal_mx(double value);
    void set_cal_cx(double value);
    void set_cal_my(double value);
    void set_cal_cy(double value);
    void set_cal_mz(double value);
    void set_cal_cz(double value);

    // Control loop timing
    void set_speed_multiplier(double value);
    void set_simulation_dt(double value);

    // PI controller settings
    void set_control_kp(double value);
    void set_control_ki(double value);

	// Save and load configuration
    void saveConfigToFile(const std::string& filename);
    void loadConfigFromFile(const std::string& filename);

    void start_program();
    void terminate_program();
private:
    std::map<std::string, double> config;
};