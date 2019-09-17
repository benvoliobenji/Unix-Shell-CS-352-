#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <iostream>

/**
 * The abstract class to be used by most of the commands in the shell (outside of a few that directly affect the shell)
 * Contains several methods that will be used to get and set information about each process as well as pure virtual function
 * that will be overridden which will contain the instructions on how to execute that function.
 * 
 * This is done to allow for easy extendability and ease of programming for the execution of the processes in the shell.
 *  
 **/
class Process
{
    private:
        pid_t pid;
        int32_t jobNumber;
        std::string command;
        std::vector<std::string> args;


    public:
        virtual int32_t execute() = 0;

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