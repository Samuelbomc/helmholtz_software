#include "GMATScripter.h"
#include "GMATScripter.h"
#include "GMATConfig.h"
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <ctime>
#include <type_traits>

namespace {
const std::string& requireConfigValue(const std::map<std::string, std::string>& config, const char* key) {
    const auto it = config.find(key);
    if (it == config.end()) {
        throw std::runtime_error(std::string("Missing required GMAT config key: ") + key);
    }
    return it->second;
}

std::string getConfigValueOrDefault(const std::map<std::string, std::string>& config, const char* key, const char* defaultValue) {
    const auto it = config.find(key);
    return it == config.end() ? std::string(defaultValue) : it->second;
}

std::string joinPath(const std::string& dir, const std::string& file) {
    if (dir.empty()) {
        return file;
    }
    if (dir.back() == '/' || dir.back() == '\\') {
        return dir + file;
    }
    return dir + "/" + file;
}

int parseIntConfigValue(const std::map<std::string, std::string>& config, const char* key) {
    const std::string& raw = requireConfigValue(config, key);
    try {
        std::size_t parsedChars = 0;
        const int value = std::stoi(raw, &parsedChars);
        if (parsedChars != raw.size()) {
            throw std::runtime_error("");
        }
        return value;
    }
    catch (...) {
        throw std::runtime_error(std::string("Invalid integer value for GMAT config key: ") + key + " ('" + raw + "')");
    }
}

std::string getCurrentLocalTimeString() {
    std::time_t t = std::time(nullptr);
    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &t);
#else
    localtime_r(&t, &local_tm);
#endif
    char created_time[32];
    std::strftime(created_time, sizeof(created_time), "%Y-%m-%d %H:%M:%S", &local_tm);
    return created_time;
}

void trimTrailingCarriageReturn(std::string& value) {
    if (!value.empty() && value.back() == '\r') {
        value.pop_back();
    }
}

template <typename T>
void setNumericConfigValue(std::map<std::string, std::string>& config, const char* key, T value) {
    static_assert(std::is_arithmetic<T>::value, "setNumericConfigValue requires arithmetic type");
    config[key] = std::to_string(value);
}

void setBoolConfigValue(std::map<std::string, std::string>& config, const char* key, bool value) {
    config[key] = value ? "true" : "false";
}
}

void GMATScripter::createScript() {
    const auto& p = config;
    const std::string& objectName = requireConfigValue(p, "ObjectName");
    const std::string& epoch = requireConfigValue(p, "Epoch");
    const std::string& sma = requireConfigValue(p, "SMA");
    const std::string& ecc = requireConfigValue(p, "ECC");
    const std::string& inc = requireConfigValue(p, "INC");
    const std::string& raan = requireConfigValue(p, "RAAN");
    const std::string& aop = requireConfigValue(p, "AOP");
    const std::string& ta = requireConfigValue(p, "TA");
    const std::string& dryMass = requireConfigValue(p, "DryMass");
    const std::string& cd = requireConfigValue(p, "Cd");
    const std::string& cr = requireConfigValue(p, "Cr");
    const std::string& dragArea = requireConfigValue(p, "DragArea");
    const std::string& srpArea = requireConfigValue(p, "SRPArea");
    const std::string& relativisticCorrection = requireConfigValue(p, "RelativisticCorrection");
    const std::string& atmosphereModel = requireConfigValue(p, "AtmosphereModel");
    const std::string& dataPath = requireConfigValue(p, "GMAT_data_path");
    const std::string& reportName = requireConfigValue(p, "report_name");
    const std::string& scriptName = requireConfigValue(p, "script_name");
    const std::string dateFormat = getConfigValueOrDefault(p, "DateFormat", "UTCGregorian");
    const std::string coordinateSystem = getConfigValueOrDefault(p, "CoordinateSystem", "EarthMJ2000Eq");
    const std::string primaryBodies = getConfigValueOrDefault(p, "PrimaryBodies", "{Earth}");
    const std::string pointMasses = getConfigValueOrDefault(p, "PointMasses", "{Luna, Sun}");
    const std::string srp = getConfigValueOrDefault(p, "SRP", "On");
    const std::string gravityDegree = getConfigValueOrDefault(p, "EarthGravityDegree", "4");
    const std::string gravityOrder = getConfigValueOrDefault(p, "EarthGravityOrder", "4");
    const std::string dragModel = getConfigValueOrDefault(p, "DragModel", "'Spherical'");
    const std::string propagatorType = getConfigValueOrDefault(p, "PropagatorType", "RungeKutta89");
    const std::string initialStepSize = getConfigValueOrDefault(p, "InitialStepSize", "60");
    const std::string accuracy = getConfigValueOrDefault(p, "Accuracy", "9.999999999999999e-12");
    const std::string minStep = getConfigValueOrDefault(p, "MinStep", "0.001");
    const std::string maxStep = getConfigValueOrDefault(p, "MaxStep", "2700");
    const std::string maxStepAttempts = getConfigValueOrDefault(p, "MaxStepAttempts", "50");
    const std::string stopIfAccuracyIsViolated = getConfigValueOrDefault(p, "StopIfAccuracyIsViolated", "true");
    const std::string reportPrecision = getConfigValueOrDefault(p, "ReportPrecision", "16");

    const int duration = parseIntConfigValue(p, "simulation_duration_sec");
    const int step = parseIntConfigValue(p, "simulation_step_sec");
    if (duration < 0) {
        throw std::runtime_error("simulation_duration_sec must be non-negative");
    }
    if (step <= 0) {
        throw std::runtime_error("simulation_step_sec must be greater than zero");
    }
    const int num_steps = duration / step;

    std::ostringstream script;

    const std::string created_time = getCurrentLocalTimeString();

    script << "%General Mission Analysis Tool(GMAT) Script\n"
        << "%Created: " << created_time << "\n\n";

    script << R"(
%----------------------------------------
%---------- Spacecraft
%----------------------------------------
Create Spacecraft )" << objectName << R"(;
GMAT )" << objectName << R"(.DateFormat = )" << dateFormat << R"(;
GMAT )" << objectName << R"(.Epoch = ')" << epoch << R"(';
GMAT )" << objectName << R"(.CoordinateSystem = )" << coordinateSystem << R"(;
GMAT )" << objectName << R"(.DisplayStateType = Keplerian;
GMAT )" << objectName << R"(.SMA = )" << sma << R"(;
GMAT )" << objectName << R"(.ECC = )" << ecc << R"(;
GMAT )" << objectName << R"(.INC = )" << inc << R"(;
GMAT )" << objectName << R"(.RAAN = )" << raan << R"(;
GMAT )" << objectName << R"(.AOP = )" << aop << R"(;
GMAT )" << objectName << R"(.TA = )" << ta << R"(;
GMAT )" << objectName << R"(.DryMass = )" << dryMass << R"(;
GMAT )" << objectName << R"(.Cd = )" << cd << R"(;
GMAT )" << objectName << R"(.Cr = )" << cr << R"(;
GMAT )" << objectName << R"(.DragArea = )" << dragArea << R"(;
GMAT )" << objectName << R"(.SRPArea = )" << srpArea << R"(;
GMAT )" << objectName << R"(.SPADDragScaleFactor = 1;
GMAT )" << objectName << R"(.SPADSRPScaleFactor = 1;
GMAT )" << objectName << R"(.AtmosDensityScaleFactor = 1;
GMAT )" << objectName << R"(.ExtendedMassPropertiesModel = 'None';
GMAT )" << objectName << R"(.NAIFId = -10000001;
GMAT )" << objectName << R"(.NAIFIdReferenceFrame = -9000001;
GMAT )" << objectName << R"(.OrbitColor = Red;
GMAT )" << objectName << R"(.TargetColor = Teal;
GMAT )" << objectName << R"(.OrbitErrorCovariance = [ 1e+70 0 0 0 0 0 ; 0 1e+70 0 0 0 0 ; 0 0 1e+70 0 0 0 ; 0 0 0 1e+70 0 0 ; 0 0 0 0 1e+70 0 ; 0 0 0 0 0 1e+70 ];
GMAT )" << objectName << R"(.CdSigma = 1e+70;
GMAT )" << objectName << R"(.CrSigma = 1e+70;
GMAT )" << objectName << R"(.Id = 'SatId';
GMAT )" << objectName << R"(.Attitude = CoordinateSystemFixed;
GMAT )" << objectName << R"(.SPADSRPInterpolationMethod = Bilinear;
GMAT )" << objectName << R"(.SPADSRPScaleFactorSigma = 1e+70;
GMAT )" << objectName << R"(.SPADDragInterpolationMethod = Bilinear;
GMAT )" << objectName << R"(.SPADDragScaleFactorSigma = 1e+70;
GMAT )" << objectName << R"(.AtmosDensityScaleFactorSigma = 1e+70;
GMAT )" << objectName << R"(.ModelFile = 'aura.3ds';
GMAT )" << objectName << R"(.ModelOffsetX = 0;
GMAT )" << objectName << R"(.ModelOffsetY = 0;
GMAT )" << objectName << R"(.ModelOffsetZ = 0;
GMAT )" << objectName << R"(.ModelRotationX = 0;
GMAT )" << objectName << R"(.ModelRotationY = 0;
GMAT )" << objectName << R"(.ModelRotationZ = 0;
GMAT )" << objectName << R"(.ModelScale = 1;
GMAT )" << objectName << R"(.AttitudeDisplayStateType = 'Quaternion';
GMAT )" << objectName << R"(.AttitudeRateDisplayStateType = 'AngularVelocity';
GMAT )" << objectName << R"(.AttitudeCoordinateSystem = )" << coordinateSystem << R"(;
GMAT )" << objectName << R"(.EulerAngleSequence = '321';
)";

    script << R"(
%----------------------------------------
%---------- ForceModels
%----------------------------------------
Create ForceModel DefaultProp_ForceModel;
GMAT DefaultProp_ForceModel.CentralBody = Earth;
GMAT DefaultProp_ForceModel.PrimaryBodies = )" << primaryBodies << R"(;
GMAT DefaultProp_ForceModel.PointMasses = )" << pointMasses << R"(;
GMAT DefaultProp_ForceModel.SRP = )" << srp << R"(;
GMAT DefaultProp_ForceModel.RelativisticCorrection = )" << relativisticCorrection << R"(;
GMAT DefaultProp_ForceModel.ErrorControl = RSSStep;
GMAT DefaultProp_ForceModel.GravityField.Earth.Degree = )" << gravityDegree << R"(;
GMAT DefaultProp_ForceModel.GravityField.Earth.Order = )" << gravityOrder << R"(;
GMAT DefaultProp_ForceModel.GravityField.Earth.StmLimit = 100;
GMAT DefaultProp_ForceModel.GravityField.Earth.PotentialFile = 'JGM3.cof';
GMAT DefaultProp_ForceModel.GravityField.Earth.TideModel = 'None';
GMAT DefaultProp_ForceModel.Drag.AtmosphereModel = )" << atmosphereModel << R"(;
GMAT DefaultProp_ForceModel.Drag.HistoricWeatherSource = 'ConstantFluxAndGeoMag';
GMAT DefaultProp_ForceModel.Drag.PredictedWeatherSource = 'ConstantFluxAndGeoMag';
GMAT DefaultProp_ForceModel.Drag.CSSISpaceWeatherFile = 'SpaceWeather-All-v1.2.txt';
GMAT DefaultProp_ForceModel.Drag.SchattenFile = 'SchattenPredict.txt';
GMAT DefaultProp_ForceModel.Drag.F107 = 150;
GMAT DefaultProp_ForceModel.Drag.F107A = 150;
GMAT DefaultProp_ForceModel.Drag.MagneticIndex = 3;
GMAT DefaultProp_ForceModel.Drag.SchattenErrorModel = 'Nominal';
GMAT DefaultProp_ForceModel.Drag.SchattenTimingModel = 'NominalCycle';
GMAT DefaultProp_ForceModel.Drag.DragModel = )" << dragModel << R"(;
GMAT DefaultProp_ForceModel.SRP.Flux = 1367;
GMAT DefaultProp_ForceModel.SRP.SRPModel = Spherical;
GMAT DefaultProp_ForceModel.SRP.Nominal_Sun = 149597870.691;
)";

    script << R"(
%----------------------------------------
%---------- Propagators
%----------------------------------------
Create Propagator DefaultProp;
GMAT DefaultProp.FM = DefaultProp_ForceModel;
GMAT DefaultProp.Type = )" << propagatorType << R"(;
GMAT DefaultProp.InitialStepSize = )" << initialStepSize << R"(;
GMAT DefaultProp.Accuracy = )" << accuracy << R"(;
GMAT DefaultProp.MinStep = )" << minStep << R"(;
GMAT DefaultProp.MaxStep = )" << maxStep << R"(;
GMAT DefaultProp.MaxStepAttempts = )" << maxStepAttempts << R"(;
GMAT DefaultProp.StopIfAccuracyIsViolated = )" << stopIfAccuracyIsViolated << R"(;
)";

    const std::string report_full_path = joinPath(dataPath, reportName);

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
GMAT ReportFile1.Precision = )" << reportPrecision << R"(;
GMAT ReportFile1.Add = {)" << objectName << R"(.UTCModJulian, )" << objectName << R"(.A1ModJulian, )" << objectName << R"(.Earth.Latitude, )" << objectName << R"(.Earth.Longitude, )" << objectName << R"(.Earth.Altitude};
GMAT ReportFile1.WriteHeaders = true;
GMAT ReportFile1.LeftJustify = On;
GMAT ReportFile1.ZeroFill = On;
GMAT ReportFile1.FixedWidth = true;
GMAT ReportFile1.Delimiter = ' ';
GMAT ReportFile1.ColumnWidth = 23;
GMAT ReportFile1.WriteReport = true;
)";

    script << R"(
%----------------------------------------
%---------- Mission Sequence
%----------------------------------------
Create Variable i;
BeginMissionSequence;
For i = 1:)" << num_steps << R"(
    Propagate DefaultProp()" << "(" << objectName << R"() ) {)" << objectName << R"(.ElapsedSecs = )" << step << R"(};
EndFor;
)";

    const std::string script_file_path = joinPath(dataPath, scriptName);

    std::ofstream out(script_file_path);
    if (!out) {
        throw std::runtime_error("Failed to open file for writing: " + script_file_path);
    }
    out << script.str();
}

void GMATScripter::setObjectName(const std::string& ObjectName) {
    config["ObjectName"] = ObjectName;
}

// Orbital Elements
void GMATScripter::setSMA(float SMA) {
    setNumericConfigValue(config, "SMA", SMA);
}
void GMATScripter::setECC(double ECC) {
    setNumericConfigValue(config, "ECC", ECC);
}
void GMATScripter::setINC(double INC) {
    setNumericConfigValue(config, "INC", INC);
}
void GMATScripter::setRAAN(double RAAN) {
    setNumericConfigValue(config, "RAAN", RAAN);
}
void GMATScripter::setAOP(double AOP) {
    setNumericConfigValue(config, "AOP", AOP);
}
void GMATScripter::setTA(double TA) {
    setNumericConfigValue(config, "TA", TA);
}

// Physical Properties
void GMATScripter::setDryMass(int DryMass) {
    setNumericConfigValue(config, "DryMass", DryMass);
}
void GMATScripter::setCd(double Cd) {
    setNumericConfigValue(config, "Cd", Cd);
}
void GMATScripter::setCr(double Cr) {
    setNumericConfigValue(config, "Cr", Cr);
}
void GMATScripter::setDragArea(double DragArea) {
    setNumericConfigValue(config, "DragArea", DragArea);
}
void GMATScripter::setSRPArea(double SRPArea) {
    setNumericConfigValue(config, "SRPArea", SRPArea);
}

// Simulation Settings
void GMATScripter::setSimulationDurationSec(int duration) {
    setNumericConfigValue(config, "simulation_duration_sec", duration);
}
void GMATScripter::setSimulationStepSec(int step) {
    setNumericConfigValue(config, "simulation_step_sec", step);
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
void GMATScripter::setSRP(const std::string& value) {
    config["SRP"] = value;
}
void GMATScripter::setPrimaryBodies(const std::string& value) {
    config["PrimaryBodies"] = value;
}
void GMATScripter::setPointMasses(const std::string& value) {
    config["PointMasses"] = value;
}
void GMATScripter::setEarthGravityDegree(int value) {
    setNumericConfigValue(config, "EarthGravityDegree", value);
}
void GMATScripter::setEarthGravityOrder(int value) {
    setNumericConfigValue(config, "EarthGravityOrder", value);
}
void GMATScripter::setDragModel(const std::string& value) {
    config["DragModel"] = value;
}

// Propagator Settings
void GMATScripter::setPropagatorType(const std::string& type) {
    config["PropagatorType"] = type;
}
void GMATScripter::setInitialStepSize(double value) {
    setNumericConfigValue(config, "InitialStepSize", value);
}
void GMATScripter::setPropagationAccuracy(double value) {
    setNumericConfigValue(config, "Accuracy", value);
}
void GMATScripter::setMinStep(double value) {
    setNumericConfigValue(config, "MinStep", value);
}
void GMATScripter::setMaxStep(double value) {
    setNumericConfigValue(config, "MaxStep", value);
}
void GMATScripter::setMaxStepAttempts(int value) {
    setNumericConfigValue(config, "MaxStepAttempts", value);
}
void GMATScripter::setStopIfAccuracyIsViolated(bool value) {
    setBoolConfigValue(config, "StopIfAccuracyIsViolated", value);
}

// Frame/Format Settings
void GMATScripter::setDateFormat(const std::string& value) {
    config["DateFormat"] = value;
}
void GMATScripter::setCoordinateSystem(const std::string& value) {
    config["CoordinateSystem"] = value;
}
void GMATScripter::setReportPrecision(int value) {
    setNumericConfigValue(config, "ReportPrecision", value);
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
    const std::string path = joinPath(std::string(TOPLEVEL_PATH) + "/data", filename + "_GMAT");
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    for (const auto& [key, value] : config) {
        out << key << "=" << value << "\n";
    }
}

void GMATScripter::loadConfigFromFile(const std::string& filename) {
    const std::string path = joinPath(std::string(TOPLEVEL_PATH) + "/data", filename + "_GMAT");
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }
    config.clear();
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        if (line.rfind("//", 0) == 0) {
            continue;
        }
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            trimTrailingCarriageReturn(value);
            config[key] = value;
        }
    }
}

std::string GMATScripter::getScriptPath() const {
    return joinPath(config.at("GMAT_data_path"), config.at("script_name"));
}