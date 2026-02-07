#pragma once
#include <map>
#include <string>
\

class PWRSRCConfig {
public:
    void set_baud_rate_sources(int baud_rate);
    void set_timeout(int timeout_ms);
    void set_max_current(double current);
    void set_max_voltage(double voltage);

    void saveConfigToFile(const std::string& filename);
    void loadConfigFromFile(const std::string& filename);

	void test_COM_port(const std::string& port);

    void connect_to_power_supply(const std::string& address);

	void set_voltage(const std::string& address, double voltage);
	void set_current(const std::string& address, double current);

	void shutdown_power_supply(const std::string& address);

private:
    std::map<std::string, std::string> config;
};