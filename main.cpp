#include "GMAT/GMATScripter.h"
#include "GMAT/GMATRunner.h"
#include "Config.h"
#include <string>

int main(int argc, char* argv[]) {
	GMATScripter scripter;
	scripter.loadConfigFromPath(std::string(TOPLEVEL_PATH) + "/data/default_test_GMAT");
	scripter.createScript(std::string(TOPLEVEL_PATH) + "/data" , "test_mission.script");

	GMATRunner runner(scripter.getScriptPath());
	runner.run_script();
}