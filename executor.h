// Author: Benjamin Vogel

#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include "process.h"

/**
 * This class is designed to execute a number of processes parsed from the Parser.
 * Within it's methods the Executor forks, schedules, and pipes the commanded processes.
 * Once the process ends (or is a background process), the parent process leaves the Executor method and returns to the shell.
 * Return methods are designed to act as "status", if it returns anything other than 0, an error has occurred within an Executor method.
 **/
class Executor
{
    public:
        /**
         * This method is called from the shell to enter the Executor. It takes in a vector of vector of Process objects and executes
         * each one in the form that the process was instructed (as given by the user). This method calls other sub-methods within
         * the Executor class to handle specific scenarios such as forground processes, background processes, and piped processes.
         * @param newProcessVector: A vector of vector of Processes to execute
         * @returns The "status" of executing the batch of processes (0 if successful, -1 if an error)
         **/
        int8_t executeBatchProcess(std::vector<std::vector<Process>> newProcessVector);

    private:
        /**
         * This method speicifically handles any number of piped processes. First, it creates the pipes for the processes to communicate,
         * then forks each process sequentially to do their work, reading and writing to their specific pipe. Finally, the parent process
         * waits for all piped child processes to end before exiting back to the shell.
         * @param pipedProcesses: A vector of Processes intended to pipe to each other
         * @returns The "staus" of executing the piped processes (0 if successful, -1 if an error)
         **/
        int8_t executePipedProcesses(std::vector<Process> pipedProcesses);

        /**
         * This method handles executing foreground processes. It will fork the parent process into two processes and then execute the child
         * process. While this happens, the parent process waits until the child process terminates before exiting the method.
         * @param foregroundProcess: The process that will be executed in the foreground
         * @returns The "status" of executing the batch of processes (0 if successful, -1 if an error)
         **/
        int8_t executeForegroundProcess(Process foregroundProcess);

        /**
         * This method handles executing background processes. It will fork the parent process into processes and then execute the child process.
         * While this happens, the parent immediately exits the method to give control back to the shell.
         * @param backgroundProcess: The process that will be executed in the background
         * @returns The "status" of forking the process (0 if successful, -1 if an error)
         **/
        int8_t executeBackgroundProcess(Process backgroundProcess);

        /**
         * The main method used to execute the processes. Both executeForegroundProcess and executeBackgroundProcesses call this method after
         * forking the two processes. This method compares the command with specific commands (ones that aren't the same as generic linux commands)
         * and executes special instructions for those. If they are not the specific commands, the method just exec()s to the desired linux command
         * and runs that.
         * @param process: The process to run
         * @returns The "status" of executing the command (0 if successful, -1 if an error)
         **/
        int8_t executeProcess(Process process);

        /**
         * A helper method designed to change the process's I/O to custom inputs or ouptuts by using dup2. The method checks to see if a new I/O file
         * is provided and if one is, changes the input or output to match the file.
         * @param process: The process to change the I/O for
         * @returns The "status" of executing the command (0 if successful, -1 if an error)
         **/
        int8_t handleIO(Process process);
};