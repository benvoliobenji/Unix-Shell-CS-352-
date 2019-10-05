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
        std::string command;
        std::vector<std::string> args;
        bool isBackground;
        std::string inputFile = "";
        std::string outputFile = "";
        bool outputFileTruncated = true;


    public:
        // TODO: Handle infile and outfile here also?
        Process(pid_t const processID, std::string const processCommand, std::vector<std::string> const commandArguments,
                bool const isBackgroundProcess)
        {
            pid = processID;
            command = processCommand;
            args = commandArguments;
            isBackground = isBackgroundProcess;
        }

        int8_t setProcessID(pid_t const newPID)
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

        int8_t setCommand(std::string newCommand) { command = newCommand; return 0;}

        int8_t setArgs(std::vector<std::string> const newArgs) { args = newArgs; return 0; }

        int8_t setBackground(bool const isBackgroundProcess) { isBackground = isBackgroundProcess; return 0; }

        int8_t setInputFile(std::string const newInputFile) { inputFile = newInputFile; return 0; }

        int8_t setOutputFile(std::string const newOutputFile) { outputFile = newOutputFile; return 0; }

        int8_t setOutputFileTruncated(bool const isTruncated) { outputFileTruncated = isTruncated; return 0;}

        const pid_t getProcessID() { return pid; }

        const std::string getCommand() { return command; }

        const std::vector<std::string> getArgs() { return args; }

        const bool getBackground() { return isBackground; }

        const std::string getInputFile() { return inputFile; }

        const std::string getOuptutFile() { return outputFile; }

        const bool getOutputFileTruncated() { return outputFileTruncated; }
};