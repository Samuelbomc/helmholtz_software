#include "GMATRunner.h"
#include "GMATConfig.h"

void GMATRunner::run_script() const{

    // --- Configuration ---
    std::string gmatConsolePath = std::string(GMAT_TOPLEVEL_PATH) + "/bin/GmatConsole.exe";

    // --- Construct the command ---
    std::string command = gmatConsolePath + " -r " + this->path;

    std::cout << "Executing GMAT command: " << command << std::endl;

    // --- Execute the command ---
    int result = std::system(command.c_str());

    // --- Check the result ---
    if (result == 0) {
        std::cout << "GMAT script executed successfully." << std::endl;
    }
    else {
        std::cerr << "Error executing GMAT script. System call returned: " << result << std::endl;
    }
}