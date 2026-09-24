#pragma once

#include <map>
#include <utility>
#include <string>

class GMATScripter {
public:
    void setConfig(const std::map<std::string, std::string>& values);
    void loadConfigFromPath(const std::string& path);

    void createScript(const std::string& outputPath, const std::string& scriptName);

    void saveConfigToFile(const std::string& filename);
    void loadConfigFromFile(const std::string& filename);

    std::string getScriptPath() const;
private:
    std::map<std::string, std::string> config;
    std::string scriptPath;
};