#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "process.h"
#include "executor.h"

class Parser 
{
    public:
        std::vector<std::vector<Process>> parseInput(std::string inputLine);

    private:
        std::vector<std::string> parseIndependentProcesses(std::string inputLine);

        std::vector<std::string> parsePipedProcesses(std::string independentProcess);

        Process parseArguments(std::string processArgs);


};
