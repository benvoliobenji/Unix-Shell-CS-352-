#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include "Process.h"

class Executor
{
    public:
        int8_t executeBatchProcess(std::vector<Process> newProcessVector);

    private:
        int8_t executeForegroundProcess(Process foregroundProcess);
        int8_t executeBackgroundProcess(Process backgroundProcess);
};