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

    const std::string& dateFormat = requireConfigValue(p, "DateFormat");
    const std::string& coordinateSystem = requireConfigValue(p, "CoordinateSystem");
    const std::string& displayStateType = requireConfigValue(p, "DisplayStateType");
    const std::string& spadDragScaleFactor = requireConfigValue(p, "SPADDragScaleFactor");
    const std::string& spadSrpScaleFactor = requireConfigValue(p, "SPADSRPScaleFactor");
    const std::string& atmosDensityScaleFactor = requireConfigValue(p, "AtmosDensityScaleFactor");
    const std::string& extendedMassPropertiesModel = requireConfigValue(p, "ExtendedMassPropertiesModel");
    const std::string& naifId = requireConfigValue(p, "NAIFId");
    const std::string& naifIdReferenceFrame = requireConfigValue(p, "NAIFIdReferenceFrame");
    const std::string& orbitColor = requireConfigValue(p, "OrbitColor");
    const std::string& targetColor = requireConfigValue(p, "TargetColor");
    const std::string& orbitErrorCovariance = requireConfigValue(p, "OrbitErrorCovariance");
    const std::string& cdSigma = requireConfigValue(p, "CdSigma");
    const std::string& crSigma = requireConfigValue(p, "CrSigma");
    const std::string& spacecraftId = requireConfigValue(p, "Id");
    const std::string& attitude = requireConfigValue(p, "Attitude");
    const std::string& spadSrpInterpolationMethod = requireConfigValue(p, "SPADSRPInterpolationMethod");
    const std::string& spadSrpScaleFactorSigma = requireConfigValue(p, "SPADSRPScaleFactorSigma");
    const std::string& spadDragInterpolationMethod = requireConfigValue(p, "SPADDragInterpolationMethod");
    const std::string& spadDragScaleFactorSigma = requireConfigValue(p, "SPADDragScaleFactorSigma");
    const std::string& atmosDensityScaleFactorSigma = requireConfigValue(p, "AtmosDensityScaleFactorSigma");
    const std::string& modelFile = requireConfigValue(p, "ModelFile");
    const std::string& modelOffsetX = requireConfigValue(p, "ModelOffsetX");
    const std::string& modelOffsetY = requireConfigValue(p, "ModelOffsetY");
    const std::string& modelOffsetZ = requireConfigValue(p, "ModelOffsetZ");
    const std::string& modelRotationX = requireConfigValue(p, "ModelRotationX");
    const std::string& modelRotationY = requireConfigValue(p, "ModelRotationY");
    const std::string& modelRotationZ = requireConfigValue(p, "ModelRotationZ");
    const std::string& modelScale = requireConfigValue(p, "ModelScale");
    const std::string& attitudeDisplayStateType = requireConfigValue(p, "AttitudeDisplayStateType");
    const std::string& attitudeRateDisplayStateType = requireConfigValue(p, "AttitudeRateDisplayStateType");
    const std::string& eulerAngleSequence = requireConfigValue(p, "EulerAngleSequence");

    const std::string& centralBody = requireConfigValue(p, "CentralBody");
    const std::string& primaryBodies = requireConfigValue(p, "PrimaryBodies");
    const std::string& pointMasses = requireConfigValue(p, "PointMasses");
    const std::string& srp = requireConfigValue(p, "SRP");
    const std::string& errorControl = requireConfigValue(p, "ErrorControl");
    const std::string& gravityDegree = requireConfigValue(p, "EarthGravityDegree");
    const std::string& gravityOrder = requireConfigValue(p, "EarthGravityOrder");
    const std::string& gravityStmLimit = requireConfigValue(p, "EarthGravityStmLimit");
    const std::string& gravityPotentialFile = requireConfigValue(p, "EarthGravityPotentialFile");
    const std::string& gravityTideModel = requireConfigValue(p, "EarthGravityTideModel");
    const std::string& historicWeatherSource = requireConfigValue(p, "HistoricWeatherSource");
    const std::string& predictedWeatherSource = requireConfigValue(p, "PredictedWeatherSource");
    const std::string& cssiSpaceWeatherFile = requireConfigValue(p, "CSSISpaceWeatherFile");
    const std::string& schattenFile = requireConfigValue(p, "SchattenFile");
    const std::string& f107 = requireConfigValue(p, "F107");
    const std::string& f107a = requireConfigValue(p, "F107A");
    const std::string& magneticIndex = requireConfigValue(p, "MagneticIndex");
    const std::string& schattenErrorModel = requireConfigValue(p, "SchattenErrorModel");
    const std::string& schattenTimingModel = requireConfigValue(p, "SchattenTimingModel");
    const std::string& dragModel = requireConfigValue(p, "DragModel");
    const std::string& srpFlux = requireConfigValue(p, "SRPFlux");
    const std::string& srpModel = requireConfigValue(p, "SRPModel");
    const std::string& nominalSun = requireConfigValue(p, "SRPNominalSun");

    const std::string& propagatorType = requireConfigValue(p, "PropagatorType");
    const std::string& initialStepSize = requireConfigValue(p, "InitialStepSize");
    const std::string& accuracy = requireConfigValue(p, "Accuracy");
    const std::string& minStep = requireConfigValue(p, "MinStep");
    const std::string& maxStep = requireConfigValue(p, "MaxStep");
    const std::string& maxStepAttempts = requireConfigValue(p, "MaxStepAttempts");
    const std::string& stopIfAccuracyIsViolated = requireConfigValue(p, "StopIfAccuracyIsViolated");

    const std::string& reportSolverIterations = requireConfigValue(p, "ReportSolverIterations");
    const std::string& reportUpperLeft = requireConfigValue(p, "ReportUpperLeft");
    const std::string& reportSize = requireConfigValue(p, "ReportSize");
    const std::string& reportRelativeZOrder = requireConfigValue(p, "ReportRelativeZOrder");
    const std::string& reportMaximized = requireConfigValue(p, "ReportMaximized");
    const std::string& reportPrecision = requireConfigValue(p, "ReportPrecision");
    const std::string& reportAdd = requireConfigValue(p, "ReportAdd");
    const std::string& reportWriteHeaders = requireConfigValue(p, "ReportWriteHeaders");
    const std::string& reportLeftJustify = requireConfigValue(p, "ReportLeftJustify");
    const std::string& reportZeroFill = requireConfigValue(p, "ReportZeroFill");
    const std::string& reportFixedWidth = requireConfigValue(p, "ReportFixedWidth");
    const std::string& reportDelimiter = requireConfigValue(p, "ReportDelimiter");
    const std::string& reportColumnWidth = requireConfigValue(p, "ReportColumnWidth");
    const std::string& reportWriteReport = requireConfigValue(p, "ReportWriteReport");

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
GMAT )" << objectName << R"(.DisplayStateType = )" << displayStateType << R"(;
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
GMAT )" << objectName << R"(.SPADDragScaleFactor = )" << spadDragScaleFactor << R"(;
GMAT )" << objectName << R"(.SPADSRPScaleFactor = )" << spadSrpScaleFactor << R"(;
GMAT )" << objectName << R"(.AtmosDensityScaleFactor = )" << atmosDensityScaleFactor << R"(;
GMAT )" << objectName << R"(.ExtendedMassPropertiesModel = )" << extendedMassPropertiesModel << R"(;
GMAT )" << objectName << R"(.NAIFId = )" << naifId << R"(;
GMAT )" << objectName << R"(.NAIFIdReferenceFrame = )" << naifIdReferenceFrame << R"(;
GMAT )" << objectName << R"(.OrbitColor = )" << orbitColor << R"(;
GMAT )" << objectName << R"(.TargetColor = )" << targetColor << R"(;
GMAT )" << objectName << R"(.OrbitErrorCovariance = )" << orbitErrorCovariance << R"(;
GMAT )" << objectName << R"(.CdSigma = )" << cdSigma << R"(;
GMAT )" << objectName << R"(.CrSigma = )" << crSigma << R"(;
GMAT )" << objectName << R"(.Id = )" << spacecraftId << R"(;
GMAT )" << objectName << R"(.Attitude = )" << attitude << R"(;
GMAT )" << objectName << R"(.SPADSRPInterpolationMethod = )" << spadSrpInterpolationMethod << R"(;
GMAT )" << objectName << R"(.SPADSRPScaleFactorSigma = )" << spadSrpScaleFactorSigma << R"(;
GMAT )" << objectName << R"(.SPADDragInterpolationMethod = )" << spadDragInterpolationMethod << R"(;
GMAT )" << objectName << R"(.SPADDragScaleFactorSigma = )" << spadDragScaleFactorSigma << R"(;
GMAT )" << objectName << R"(.AtmosDensityScaleFactorSigma = )" << atmosDensityScaleFactorSigma << R"(;
GMAT )" << objectName << R"(.ModelFile = )" << modelFile << R"(;
GMAT )" << objectName << R"(.ModelOffsetX = )" << modelOffsetX << R"(;
GMAT )" << objectName << R"(.ModelOffsetY = )" << modelOffsetY << R"(;
GMAT )" << objectName << R"(.ModelOffsetZ = )" << modelOffsetZ << R"(;
GMAT )" << objectName << R"(.ModelRotationX = )" << modelRotationX << R"(;
GMAT )" << objectName << R"(.ModelRotationY = )" << modelRotationY << R"(;
GMAT )" << objectName << R"(.ModelRotationZ = )" << modelRotationZ << R"(;
GMAT )" << objectName << R"(.ModelScale = )" << modelScale << R"(;
GMAT )" << objectName << R"(.AttitudeDisplayStateType = )" << attitudeDisplayStateType << R"(;
GMAT )" << objectName << R"(.AttitudeRateDisplayStateType = )" << attitudeRateDisplayStateType << R"(;
GMAT )" << objectName << R"(.AttitudeCoordinateSystem = )" << coordinateSystem << R"(;
GMAT )" << objectName << R"(.EulerAngleSequence = )" << eulerAngleSequence << R"(;
)";

    script << R"(
%----------------------------------------
%---------- ForceModels
%----------------------------------------
Create ForceModel DefaultProp_ForceModel;
GMAT DefaultProp_ForceModel.CentralBody = )" << centralBody << R"(;
GMAT DefaultProp_ForceModel.PrimaryBodies = )" << primaryBodies << R"(;
GMAT DefaultProp_ForceModel.PointMasses = )" << pointMasses << R"(;
GMAT DefaultProp_ForceModel.SRP = )" << srp << R"(;
GMAT DefaultProp_ForceModel.RelativisticCorrection = )" << relativisticCorrection << R"(;
GMAT DefaultProp_ForceModel.ErrorControl = )" << errorControl << R"(;
GMAT DefaultProp_ForceModel.GravityField.Earth.Degree = )" << gravityDegree << R"(;
GMAT DefaultProp_ForceModel.GravityField.Earth.Order = )" << gravityOrder << R"(;
GMAT DefaultProp_ForceModel.GravityField.Earth.StmLimit = )" << gravityStmLimit << R"(;
GMAT DefaultProp_ForceModel.GravityField.Earth.PotentialFile = )" << gravityPotentialFile << R"(;
GMAT DefaultProp_ForceModel.GravityField.Earth.TideModel = )" << gravityTideModel << R"(;
GMAT DefaultProp_ForceModel.Drag.AtmosphereModel = )" << atmosphereModel << R"(;
GMAT DefaultProp_ForceModel.Drag.HistoricWeatherSource = )" << historicWeatherSource << R"(;
GMAT DefaultProp_ForceModel.Drag.PredictedWeatherSource = )" << predictedWeatherSource << R"(;
GMAT DefaultProp_ForceModel.Drag.CSSISpaceWeatherFile = )" << cssiSpaceWeatherFile << R"(;
GMAT DefaultProp_ForceModel.Drag.SchattenFile = )" << schattenFile << R"(;
GMAT DefaultProp_ForceModel.Drag.F107 = )" << f107 << R"(;
GMAT DefaultProp_ForceModel.Drag.F107A = )" << f107a << R"(;
GMAT DefaultProp_ForceModel.Drag.MagneticIndex = )" << magneticIndex << R"(;
GMAT DefaultProp_ForceModel.Drag.SchattenErrorModel = )" << schattenErrorModel << R"(;
GMAT DefaultProp_ForceModel.Drag.SchattenTimingModel = )" << schattenTimingModel << R"(;
GMAT DefaultProp_ForceModel.Drag.DragModel = )" << dragModel << R"(;
GMAT DefaultProp_ForceModel.SRP.Flux = )" << srpFlux << R"(;
GMAT DefaultProp_ForceModel.SRP.SRPModel = )" << srpModel << R"(;
GMAT DefaultProp_ForceModel.SRP.Nominal_Sun = )" << nominalSun << R"(;
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
GMAT ReportFile1.SolverIterations = )" << reportSolverIterations << R"(;
GMAT ReportFile1.UpperLeft = )" << reportUpperLeft << R"(;
GMAT ReportFile1.Size = )" << reportSize << R"(;
GMAT ReportFile1.RelativeZOrder = )" << reportRelativeZOrder << R"(;
GMAT ReportFile1.Maximized = )" << reportMaximized << R"(;
GMAT ReportFile1.Filename = ')" << report_full_path << R"(';
GMAT ReportFile1.Precision = )" << reportPrecision << R"(;
GMAT ReportFile1.Add = )" << reportAdd << R"(;
GMAT ReportFile1.WriteHeaders = )" << reportWriteHeaders << R"(;
GMAT ReportFile1.LeftJustify = )" << reportLeftJustify << R"(;
GMAT ReportFile1.ZeroFill = )" << reportZeroFill << R"(;
GMAT ReportFile1.FixedWidth = )" << reportFixedWidth << R"(;
GMAT ReportFile1.Delimiter = )" << reportDelimiter << R"(;
GMAT ReportFile1.ColumnWidth = )" << reportColumnWidth << R"(;
GMAT ReportFile1.WriteReport = )" << reportWriteReport << R"(;
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