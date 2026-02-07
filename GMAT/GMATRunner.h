#pragma once
#include <iostream>
#include <string>
#include <cstdlib>

class GMATRunner {
public:
	GMATRunner(std::string path) : path(std::move(path)) {}

	void run_script() const;
private:
	std::string path;
};