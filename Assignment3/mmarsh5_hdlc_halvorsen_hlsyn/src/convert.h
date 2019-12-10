
#pragma once

#include <sstream>

#define SUCCESS 0
#define FAILURE 1

int convertToVerilog(std::stringstream &in, std::stringstream &out, std::string outFileName, int maxCycles);
