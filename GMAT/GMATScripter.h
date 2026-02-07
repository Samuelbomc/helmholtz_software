#pragma once
#include <map>
#include <utility>
#include <string>

class GMATScripter {
public:

	void setObjectName(std::string& ObjectName);

	// Orbital Elements
	void setSMA(float SMA);
	void setECC(double ECC);
	void setINC(double INC);
	void setRAAN(double RAAN);
	void setAOP(double AOP);
	void setTA(double TA);

	// Physical Properties
	void setDryMass(int DryMass);
	void setCd(double Cd);
	void setCr(double Cr);
	void setDragArea(double DragArea);
	void setSRPArea(double SRPArea);

	// Simulation Settings
	void setSimulationDurationSec(int duration);
	void setSimulationStepSec(int step);
	void setEpoch(const std::string& Epoch);

	// Force Model Settings
	void setRelativisticCorrection(const std::string& value);
	void setAtmosphereModel(const std::string& model);

	// File Paths
	void setDataPath(const std::string& path);
	void setReportName(const std::string& name);
	void setScriptName(const std::string& name);

	void createScript();

	void saveConfigToFile(const std::string& filename);
	void loadConfigFromFile(const std::string& filename);
	
	std::string getScriptPath() const;
private:
	std::map<std::string, std::string> config;
};