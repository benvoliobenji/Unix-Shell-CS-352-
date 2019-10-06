// Author: Benjamin Vogel
#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "process.h"
#include "executor.h"

/**
 * This class is designed to take in individual lines from the main shell command and parse them
 * into individual Processes for the Executor to execute. Within it's methods the Parser parses
 * independent processes, piped processes, and arguments for those processes which will be stored
 * in the Process class object.
 **/
class Parser 
{
    public:
        /**
         * This is the generic method to be called by the shell. In order to minimize method calls
         * and complexity within the shell, this method calls it's helper functions in order to parse
         * the line if input provided by the shell into individual and piped processes.
         * @param inputLine: The line of text received by the shell either from a file or the user to parse into Processes
         * @returns A vector of vector of Processes to be executed by the Executor.
         **/
        std::vector<std::vector<Process>> parseInput(std::string inputLine);

    private:
        /**
         * This method splits up a given string along the ";" character and returns a vector of the split strings.
         * This is done because the shell operates as if ";" marks the separation between independent processes, where the
         * process before and after the ";" do not communicate to each other through a pipe at all.
         * @param inputLine: The line of inputs give to parseInput() by the shell
         * @returns A vector of strings, where each string is an independent process
         **/
        std::vector<std::string> parseIndependentProcesses(std::string inputLine);

        /**
         * This method splits up the independent processes into separate piped processes. If the string has no pipe
         * delmiter "|", the returned vector is only of size 1. However, if there are piped processes, this will return
         * a vector of processes that need to be piped to each other in the Executor.
         * @param independentProcess: A string denoting an independent process (returned in parseIndependentProcesses())
         * @returns A vector of strings where each string is a piped process (unless the size is 1)
         **/
        std::vector<std::string> parsePipedProcesses(std::string independentProcess);

        /**
         * This method parses the leftover string into individual arguments such as the infile, outfile, if the process is background or not,
         * and command arguments needed for execution and stores them in the Process data structure.
         * @param processArgs: The string of arguments needed by the Executor to properly execute the process
         * @returns A Process object containing the arguments needed for execution
         **/
        Process parseArguments(std::string processArgs);


};
