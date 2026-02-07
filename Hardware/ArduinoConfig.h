#pragma once
#include <map>
#include <string>
#include <vector>

class ArduinoConfig {
public:
    void set_serial_timeout(double timeout);
    void set_mag_baud_rate(int baud_rate);
    void set_relay_baud_rate(int baud_rate);
    void set_magnetometer_port(const std::string& port);
    void set_relay_port(const std::string& port);

	void saveConfigToFile(const std::string& filename);
	void loadConfigFromFile(const std::string& filename);

	void test_COM_port(const std::string& port);

	void connect_to_magnetometer();
	void connect_to_relay();

	void set_relay_state(bool state_x, bool state_y, bool state_z);
	std::vector<double> read_magnetometer_data();

	void close_connections();
	
private:
	std::map<std::string, std::string> config;
};