#include "GMATScripter.h"
#include "GMATConfig.h"
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <ctime>
#include <iostream>

void GMATScripter::createScript() {
    const auto& p = config;
    std::ostringstream script;

    // Get current time for script title
    std::time_t t = std::time(nullptr);
    char created_time[32];
    std::strftime(created_time, sizeof(created_time), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    // Script title
    script << "%General Mission Analysis Tool(GMAT) Script\n"
        << "%Created: " << created_time << "\n\n";

    // Spacecraft block
    script << R"(
%----------------------------------------
%---------- Spacecraft
%----------------------------------------
Create Spacecraft )" << p.at("ObjectName") << R"(;
GMAT )" << p.at("ObjectName") << R"(.DateFormat = UTCGregorian;
GMAT )" << p.at("ObjectName") << R"(.Epoch = ')" << p.at("Epoch") << R"(';
GMAT )" << p.at("ObjectName") << R"(.CoordinateSystem = EarthMJ2000Eq;
GMAT )" << p.at("ObjectName") << R"(.DisplayStateType = Keplerian;
GMAT )" << p.at("ObjectName") << R"(.SMA = )" << p.at("SMA") << R"(;
GMAT )" << p.at("ObjectName") << R"(.ECC = )" << p.at("ECC") << R"(;
GMAT )" << p.at("ObjectName") << R"(.INC = )" << p.at("INC") << R"(;
GMAT )" << p.at("ObjectName") << R"(.RAAN = )" << p.at("RAAN") << R"(;
GMAT )" << p.at("ObjectName") << R"(.AOP = )" << p.at("AOP") << R"(;
GMAT )" << p.at("ObjectName") << R"(.TA = )" << p.at("TA") << R"(;
GMAT )" << p.at("ObjectName") << R"(.DryMass = )" << p.at("DryMass") << R"(;
GMAT )" << p.at("ObjectName") << R"(.Cd = )" << p.at("Cd") << R"(;
GMAT )" << p.at("ObjectName") << R"(.Cr = )" << p.at("Cr") << R"(;
GMAT )" << p.at("ObjectName") << R"(.DragArea = )" << p.at("DragArea") << R"(;
GMAT )" << p.at("ObjectName") << R"(.SRPArea = 1;
GMAT )" << p.at("ObjectName") << R"(.SPADDragScaleFactor = 1;
GMAT )" << p.at("ObjectName") << R"(.SPADSRPScaleFactor = 1;
GMAT )" << p.at("ObjectName") << R"(.AtmosDensityScaleFactor = 1;
GMAT )" << p.at("ObjectName") << R"(.ExtendedMassPropertiesModel = 'None';
GMAT )" << p.at("ObjectName") << R"(.NAIFId = -10000001;
GMAT )" << p.at("ObjectName") << R"(.NAIFIdReferenceFrame = -9000001;
GMAT )" << p.at("ObjectName") << R"(.OrbitColor = Red;
GMAT )" << p.at("ObjectName") << R"(.TargetColor = Teal;
GMAT )" << p.at("ObjectName") << R"(.OrbitErrorCovariance = [ 1e+70 0 0 0 0 0 ; 0 1e+70 0 0 0 0 ; 0 0 1e+70 0 0 0 ; 0 0 0 1e+70 0 0 ; 0 0 0 0 1e+70 0 ; 0 0 0 0 0 1e+70 ];
GMAT )" << p.at("ObjectName") << R"(.CdSigma = 1e+70;
GMAT )" << p.at("ObjectName") << R"(.CrSigma = 1e+70;
GMAT )" << p.at("ObjectName") << R"(.Id = 'SatId';
GMAT )" << p.at("ObjectName") << R"(.Attitude = CoordinateSystemFixed;
GMAT )" << p.at("ObjectName") << R"(.SPADSRPInterpolationMethod = Bilinear;
GMAT )" << p.at("ObjectName") << R"(.SPADSRPScaleFactorSigma = 1e+70;
GMAT )" << p.at("ObjectName") << R"(.SPADDragInterpolationMethod = Bilinear;
GMAT )" << p.at("ObjectName") << R"(.SPADDragScaleFactorSigma = 1e+70;
GMAT )" << p.at("ObjectName") << R"(.AtmosDensityScaleFactorSigma = 1e+70;
GMAT )" << p.at("ObjectName") << R"(.ModelFile = 'aura.3ds';
GMAT )" << p.at("ObjectName") << R"(.ModelOffsetX = 0;
GMAT )" << p.at("ObjectName") << R"(.ModelOffsetY = 0;
GMAT )" << p.at("ObjectName") << R"(.ModelOffsetZ = 0;
GMAT )" << p.at("ObjectName") << R"(.ModelRotationX = 0;
GMAT )" << p.at("ObjectName") << R"(.ModelRotationY = 0;
GMAT )" << p.at("ObjectName") << R"(.ModelRotationZ = 0;
GMAT )" << p.at("ObjectName") << R"(.ModelScale = 1;
GMAT )" << p.at("ObjectName") << R"(.AttitudeDisplayStateType = 'Quaternion';
GMAT )" << p.at("ObjectName") << R"(.AttitudeRateDisplayStateType = 'AngularVelocity';
GMAT )" << p.at("ObjectName") << R"(.AttitudeCoordinateSystem = EarthMJ2000Eq;
GMAT )" << p.at("ObjectName") << R"(.EulerAngleSequence = '321';
)";

    // Force Models block
    script << R"(
%----------------------------------------
%---------- ForceModels
%----------------------------------------
Create ForceModel DefaultProp_ForceModel;
GMAT DefaultProp_ForceModel.CentralBody = Earth;
GMAT DefaultProp_ForceModel.PrimaryBodies = {Earth};
GMAT DefaultProp_ForceModel.PointMasses = {Luna, Sun};
GMAT DefaultProp_ForceModel.SRP = On;
GMAT DefaultProp_ForceModel.RelativisticCorrection = )" << p.at("RelativisticCorrection") << R"(;
GMAT DefaultProp_ForceModel.ErrorControl = RSSStep;
GMAT DefaultProp_ForceModel.GravityField.Earth.Degree = 4;
GMAT DefaultProp_ForceModel.GravityField.Earth.Order = 4;
GMAT DefaultProp_ForceModel.GravityField.Earth.StmLimit = 100;
GMAT DefaultProp_ForceModel.GravityField.Earth.PotentialFile = 'JGM3.cof';
GMAT DefaultProp_ForceModel.GravityField.Earth.TideModel = 'None';
GMAT DefaultProp_ForceModel.Drag.AtmosphereModel = )" << p.at("AtmosphereModel") << R"(;
GMAT DefaultProp_ForceModel.Drag.HistoricWeatherSource = 'ConstantFluxAndGeoMag';
GMAT DefaultProp_ForceModel.Drag.PredictedWeatherSource = 'ConstantFluxAndGeoMag';
GMAT DefaultProp_ForceModel.Drag.CSSISpaceWeatherFile = 'SpaceWeather-All-v1.2.txt';
GMAT DefaultProp_ForceModel.Drag.SchattenFile = 'SchattenPredict.txt';
GMAT DefaultProp_ForceModel.Drag.F107 = 150;
GMAT DefaultProp_ForceModel.Drag.F107A = 150;
GMAT DefaultProp_ForceModel.Drag.MagneticIndex = 3;
GMAT DefaultProp_ForceModel.Drag.SchattenErrorModel = 'Nominal';
GMAT DefaultProp_ForceModel.Drag.SchattenTimingModel = 'NominalCycle';
GMAT DefaultProp_ForceModel.Drag.DragModel = 'Spherical';
GMAT DefaultProp_ForceModel.SRP.Flux = 1367;
GMAT DefaultProp_ForceModel.SRP.SRPModel = Spherical;
GMAT DefaultProp_ForceModel.SRP.Nominal_Sun = 149597870.691;
)";

    // Propagators block
    script << R"(
%----------------------------------------
%---------- Propagators
%----------------------------------------
Create Propagator DefaultProp;
GMAT DefaultProp.FM = DefaultProp_ForceModel;
GMAT DefaultProp.Type = RungeKutta89;
GMAT DefaultProp.InitialStepSize = 60;
GMAT DefaultProp.Accuracy = 9.999999999999999e-12;
GMAT DefaultProp.MinStep = 0.001;
GMAT DefaultProp.MaxStep = 2700;
GMAT DefaultProp.MaxStepAttempts = 50;
GMAT DefaultProp.StopIfAccuracyIsViolated = true;
)";

    // Subscribers block
    std::string report_full_path = p.at("GMAT_data_path") + "/" + p.at("report_name");

    script << R"(
%----------------------------------------
%---------- Subscribers
%----------------------------------------
Create ReportFile ReportFile1;
GMAT ReportFile1.SolverIterations = Current;
GMAT ReportFile1.UpperLeft = [ 0 0 ];
GMAT ReportFile1.Size = [ 0 0 ];
GMAT ReportFile1.RelativeZOrder = 0;
GMAT ReportFile1.Maximized = false;
GMAT ReportFile1.Filename = ')" << report_full_path << R"(';
GMAT ReportFile1.Precision = 16;
GMAT ReportFile1.Add = {)" << p.at("ObjectName") << R"(.UTCModJulian, )" << p.at("ObjectName") << R"(.A1ModJulian, )" << p.at("ObjectName") << R"(.Earth.Latitude, )" << p.at("ObjectName") << R"(.Earth.Longitude, )" << p.at("ObjectName") << R"(.Earth.Altitude};
GMAT ReportFile1.WriteHeaders = true;
GMAT ReportFile1.LeftJustify = On;
GMAT ReportFile1.ZeroFill = On;
GMAT ReportFile1.FixedWidth = true;
GMAT ReportFile1.Delimiter = ' ';
GMAT ReportFile1.ColumnWidth = 23;
GMAT ReportFile1.WriteReport = true;
)";

    // Mission Sequence block
    int duration = std::stoi(p.at("simulation_duration_sec"));
    int step = std::stoi(p.at("simulation_step_sec"));
    int num_steps = duration / step;

    script << R"(
%----------------------------------------
%---------- Mission Sequence
%----------------------------------------
Create Variable i;
BeginMissionSequence;
For i = 1:)" << num_steps << R"(
    Propagate DefaultProp()" << "(" << p.at("ObjectName") << R"() ) {)" << p.at("ObjectName") << R"(.ElapsedSecs = )" << step << R"(};
EndFor;
)";

    // Save script at GMAT_data_path
    std::string script_file_path = p.at("GMAT_data_path") + "/" + p.at("script_name");

    // DEBUG PRINT
	std::cout << "Saving GMAT script to: " << script_file_path << std::endl;

    std::ofstream out(script_file_path);
    if (!out) {
        throw std::runtime_error("Failed to open file for writing: " + script_file_path);
    }
    out << script.str();
    out.close();
}

void GMATScripter::setObjectName(std::string& ObjectName) {
    config["ObjectName"] = ObjectName;
}

// Orbital Elements
void GMATScripter::setSMA(float SMA) {
    config["SMA"] = std::to_string(SMA);
}
void GMATScripter::setECC(double ECC) {
    config["ECC"] = std::to_string(ECC);
}
void GMATScripter::setINC(double INC) {
    config["INC"] = std::to_string(INC);
}
void GMATScripter::setRAAN(double RAAN) {
    config["RAAN"] = std::to_string(RAAN);
}
void GMATScripter::setAOP(double AOP) {
    config["AOP"] = std::to_string(AOP);
}
void GMATScripter::setTA(double TA) {
    config["TA"] = std::to_string(TA);
}

// Physical Properties
void GMATScripter::setDryMass(int DryMass) {
    config["DryMass"] = std::to_string(DryMass);
}
void GMATScripter::setCd(double Cd) {
    config["Cd"] = std::to_string(Cd);
}
void GMATScripter::setCr(double Cr) {
    config["Cr"] = std::to_string(Cr);
}
void GMATScripter::setDragArea(double DragArea) {
    config["DragArea"] = std::to_string(DragArea);
}
void GMATScripter::setSRPArea(double SRPArea) {
    config["SRPArea"] = std::to_string(SRPArea);
}

// Simulation Settings
void GMATScripter::setSimulationDurationSec(int duration) {
    config["simulation_duration_sec"] = std::to_string(duration);
}
void GMATScripter::setSimulationStepSec(int step) {
    config["simulation_step_sec"] = std::to_string(step);
}
void GMATScripter::setEpoch(const std::string& Epoch) {
    config["Epoch"] = Epoch;
}

// Force Model Settings
void GMATScripter::setRelativisticCorrection(const std::string& value) {
    config["RelativisticCorrection"] = value;
}
void GMATScripter::setAtmosphereModel(const std::string& model) {
    config["AtmosphereModel"] = model;
}

// File Paths
void GMATScripter::setDataPath(const std::string& path) {
    config["GMAT_data_path"] = path;
}
void GMATScripter::setReportName(const std::string& name) {
    config["report_name"] = name;
}
void GMATScripter::setScriptName(const std::string& name) {
    config["script_name"] = name;
}

void GMATScripter::saveConfigToFile(const std::string& filename) {
    std::ofstream out(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_GMAT");
    if (!out) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    for (const auto& [key, value] : config) {
        out << key << "=" << value << "\n";
    }
    out.close();
}

void GMATScripter::loadConfigFromFile(const std::string& filename) {
    std::ifstream in(std::string(TOPLEVEL_PATH) + "/data/" + filename + "_GMAT");
    if (!in) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }
    config.clear();
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            config[key] = value;
        }
    }
    in.close();
}

std::string GMATScripter::getScriptPath() const {
    return config.at("GMAT_data_path") + "/" + config.at("script_name");
}