#pragma once

#pragma once

#include <map>
#include <utility>
#include <string>

class GMATScripter {
public:

    void setObjectName(const std::string& ObjectName);

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
	void setSRP(const std::string& value);
	void setPrimaryBodies(const std::string& value);
	void setPointMasses(const std::string& value);
	void setEarthGravityDegree(int value);
	void setEarthGravityOrder(int value);
	void setDragModel(const std::string& value);

	// Propagator Settings
	void setPropagatorType(const std::string& type);
	void setInitialStepSize(double value);
	void setPropagationAccuracy(double value);
	void setMinStep(double value);
	void setMaxStep(double value);
	void setMaxStepAttempts(int value);
	void setStopIfAccuracyIsViolated(bool value);

	// Frame/Format Settings
	void setDateFormat(const std::string& value);
	void setCoordinateSystem(const std::string& value);
	void setReportPrecision(int value);

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