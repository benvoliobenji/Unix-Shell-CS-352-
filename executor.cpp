// Author: Benjamin Vogel
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"

/**
 * This file contains the implementation of the Executor methods decribed in "executor.h"
 **/

int8_t Executor::executeBatchProcess(std::vector<std::vector<Process>> newProcessVector)
{
    // Check to make sure there are actually new processes to execute
    if (newProcessVector.size()  == 0)
    {
        return -1;
    }

    auto processVectorIterator = newProcessVector.begin();

    for(; processVectorIterator != newProcessVector.end(); ++processVectorIterator)
    {
        int processResult = 0;

        if ((*processVectorIterator).size() > 1)
        {
            // If we have a vector size of more than one, that means the parser found a piped process
            processResult = executePipedProcesses(*processVectorIterator);
        }
        else
        {
            Process individualProcess = (*processVectorIterator)[0];

            if (individualProcess.getCommand().compare("cd") == 0)
            {
                // "cd" directly affects the parent process, so do it here rather than in a child process

                std::vector<std::string> args = individualProcess.getArgs();
                // Since we know it's only one argument which is the path to change the directory to,
                // we can just grab that sole argument

                if (args.size() == 1)
                {
                    // We didn't have any arguments, so just print out the current directory
                    perror("No directory listed");
                    return -1;
                }


                int result = chdir(args[1].c_str());

                if (result < 0)
                {
                    // We could not change the directory, so throw an error
                    perror("Directory");
                    processResult = -1;
                }
                else
                {
                    // Changing directory was successful, print new directory
                    char* newDirectory = (char*) malloc(256);
                    getcwd(newDirectory, 256);
                    std::cout << newDirectory << std::endl;
                    processResult = 0;
                }
            }
            else if (individualProcess.getBackground() == false)
            {
                // This means we have a foreground process
                processResult = executeForegroundProcess(individualProcess);
            }
            else
            {
                // This is a background process
                processResult = executeBackgroundProcess(individualProcess);
            }  
        }

        if (processResult != 0)
        {
            // This means we had an error in executing a process, so abandon the batch of processes and return up to the shell
            return processResult;
        }
    }

    // Everything executed as planned, so return a 0
    return 0;
}

int8_t Executor::executePipedProcesses(std::vector<Process> pipedProcesses)
{
    int numPipes = (pipedProcesses.size() - 1);

    int status;
    int i = 0;
    pid_t pid;

    // Our array of pipes
    int pipefds[2 * numPipes];

    // Create pipes
    for(i = 0; i < (numPipes); i++)
    {
        if(pipe(pipefds + i*2) < 0)
        {
            perror("Couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Create the pipe index and a vector of child pids
    int j = 0;
    std::vector<pid_t> child_pids;

    // Iterate through each process
    for(size_t processes = 0; processes < pipedProcesses.size(); processes++)
    {
        pid = fork();
        if (pid == 0)
        {
            if (processes == pipedProcesses.size() - 1)
            {
                // Handle file output for the final process
                handleIO(pipedProcesses[processes]);
            }

            if (processes == 0)
            {
                // Handle file input for the first process
                handleIO(pipedProcesses[0]);
            }

            // If this isn't the first command
            if (j != 0)
            {
                // Change the input to be from the pipe
                if (dup2(pipefds[(j - 1) * 2], STDIN_FILENO) < 0)
                {
                    perror("Error with dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Check to see if this is the last piped process or not
            if (processes < pipedProcesses.size() - 1)
            {
                // Change the standard out to be the pipe
                if (dup2(pipefds[(j * 2) + 1], STDOUT_FILENO) < 0)
                {
                    perror("Error with dup2");
                    exit(EXIT_FAILURE);
                }
            }

            for(i = 0; i < 2*numPipes; i++)
            {
                // Close the pipes
                close(pipefds[i]);
            }

            // Convert the args to a char*
            std::vector<std::string> args = pipedProcesses[processes].getArgs();
            std::vector<char*> cstrings{};
            
            for(auto& string: args)
            {
                cstrings.push_back(&string.front());
            }

            // Make sure we push back a NULL to satisfy arguments for the linux comands
            cstrings.push_back(NULL);

            // Execvp the process
            if (execvp(pipedProcesses[processes].getCommand().c_str(), cstrings.data()) < 0)
            {
                perror("Error with execvp");
                exit(EXIT_FAILURE);
            }
        }
        else if (pid < 0)
        {
            perror("Error forking");
            return -1;
        }
        else if (pid > 0)
        {
            // The parent needs to keep track of all the child pids for waiting
            child_pids.push_back(pid);
        }

        // Increment the pipe index
        j++;
    }

    // Close all the pipes completely
    for(i = 0; i < 2 * numPipes; i++)
    {
        close(pipefds[i]);
    }

    // Wait for each child to exit
    for(auto childPIDIterator = child_pids.begin(); childPIDIterator != child_pids.end(); ++childPIDIterator)
    {
        waitpid(*childPIDIterator, &status, 0);
    }

    return status;
}

int8_t Executor::executeForegroundProcess(Process foregroundProcess)
{
    pid_t pid = fork();

    // Check to make sure the process was correctly forked
    if (pid < 0)
    {
        perror("Forking");
        return -1;
    }

    if (pid == 0)
    {
        // We are in the child process now
       int8_t result = executeProcess(foregroundProcess);
        exit(result);
    }

    else
    {
        int status = 0;
        waitpid(pid, &status, 0);

        if (status != 0)
        {
            // perror("Child Process");
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

int8_t Executor::executeBackgroundProcess(Process backgroundProcess)
{
    pid_t pid = fork();

    // Check to make sure the process was correctly forked
    if (pid < 0)
    {
        perror("Forking");
        return -1;
    }

    if (pid == 0)
    {
        // This is the child process
        int8_t result = executeProcess(backgroundProcess);
        exit(result);
    }

    else
    {
        // Since this is a background process, just exit
        return 0;
    }
}

int8_t Executor::executeProcess(Process process)
{
    // First we handle any changes to the I/O
    int result = 0;
    result = handleIO(process);

    if (result != 0)
    {
        return result;
    }

    // Now we execute the process
    if (process.getCommand().compare("clr") == 0)
    {
        return execlp("clear", "clear", NULL);
    }
    else if (process.getCommand().compare("dir") == 0)
    {
        std::vector<std::string> args = process.getArgs();
        std::vector<char*> cstrings{};

        // Make sure to push back the command first
        // This is specifically ls because of how linux works instead

        char* command = (char *)"ls";
        cstrings.push_back(command);
        
        // Needed for a conversion from a vector of strings to a char** for execvp
        for(auto& string: args)
        {
            if (string.compare("dir") == 0)
            {
                continue;
            }
            cstrings.push_back(&string.front());
        }

        return execvp("ls", cstrings.data());
    }
    else if (process.getCommand().compare("environ") == 0)
    {
        execlp("printenv", "printenv", NULL);
    }
    else if (process.getCommand().compare("help") == 0)
    {
        // Since help is actually man help | more, we need to execute a piped process
        // We are supposed to display the user manual, so man is needed
        std::vector<Process> helpCommand;
        std::vector<std::string> helpArgs = {"man", "help"};
        Process helpProcess(getpid(), "man", helpArgs, false);

        std::vector<std::string> moreArgs = {"more"};
        Process moreProcess(getpid(), "more", moreArgs, false);

        helpCommand.push_back(helpProcess);
        helpCommand.push_back(moreProcess);

        return executePipedProcesses(helpCommand);
    }
    else
    {
        std::vector<std::string> args = process.getArgs();

        if (args.size() == 0)
        {
            // No need to go through all the copying process if it's just a solo command with no args
            execlp(process.getCommand().c_str(), process.getCommand().c_str(), NULL);
        }

        // Copy the arguments into a char *
        std::vector<char*> cstrings{};
        for(auto& string: args)
        {
            cstrings.push_back(&string.front());
        }

        // Make sure we push back a NULL to satisfy arguments for the linux comands
        cstrings.push_back(NULL);

        // Execute the process
        return execvp(process.getCommand().c_str(), cstrings.data());   
    }

    return 0;
}

int8_t Executor::handleIO(Process process)
{
    // See if we have an input file to read from
    if (process.getInputFile().size() > 0)
    {
        int file = open(process.getInputFile().c_str(), O_RDONLY);

        // Check to make sure the file was opened properly
        if (file < 0)
        {
            perror("Opening input file");
            return -1;
        } 

        else
        {
            // Change our stdin to the file
            dup2(file, STDIN_FILENO);

            // Close the file
            close(file);
        }
        
    }

    // Now check if there is an output file
    if (process.getOuptutFile().size() > 0)
    {
        int file;

        // Change the parameters when opening the file depending on if it's truncate or append
        if (process.getOutputFileTruncated())
        {
            file = open(process.getOuptutFile().c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        }
        else
        {
            file = open(process.getOuptutFile().c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        }
        

        // Check to make sure the file was opened properly
        if (file < 0)
        {
            perror("Opening output file");
            return -1;
        }

        else
        {
            // Change the stdout to this file
            dup2(file, STDOUT_FILENO);
            dup2(file, STDERR_FILENO);

            // Close the file
            close(file);
        } 
    }

    return 0;
}