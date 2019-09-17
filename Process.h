#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <iostream>

class Process
{
    private:
        pid_t pid;
        int32_t jobNumber;
        std::string command;
        std::vector<std::string> args;

    public:
        virtual int execute() = 0;

        int32_t setProcessID(pid_t const newPID)
        {
            if (newPID < 0)
            {
                std::cerr << "ERROR: Please provide a valid pid" << std::endl << std::flush;
                return -1;
            }
            else
            {
                pid = newPID;
                return 0;
            }
        }

        int32_t setJobNumber( int32_t const newJobNumber)
        {
            if (newJobNumber < 0)
            {
                std::cerr << "ERROR: Please provide a Job Number greater than 0" << std::endl << std::flush;
                return -1;
            }
            else
            {
                jobNumber = newJobNumber;
                return 0;
            }
        }

        int32_t setCommand(std::string newCommand) { command = newCommand; return 0;};

        int32_t setArgs(std::vector<std::string> const newArgs) {args = newArgs; return 0; };

        const pid_t getProcessID() { return pid; };

        const int32_t getJobNumber() { return jobNumber; };

        const std::string getCommand() { return command; };

        const std::vector<std::string> getArgs() { return args; };
};