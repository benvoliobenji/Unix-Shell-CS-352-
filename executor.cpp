#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"

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
           processResult = executePipedProcesses(*processVectorIterator);
        }
        else
        {
            Process individualProcess = (*processVectorIterator)[0];
            if (individualProcess.getBackground() == false)
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

    int pipefds[2 * numPipes];

    for(i = 0; i < (numPipes); i++)
    {
        if(pipe(pipefds + i*2) < 0)
        {
            perror("Couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }

    int j = 0;
    for(int processes = 0; processes < pipedProcesses.size(); processes++)
    {
        pid = fork();
        if (pid == 0)
        {
            if (processes == 0)
            {
                // Handle file input for the first process
                handleIO(pipedProcesses[0]);
            }

            // Check to see if this is the last piped process or not
            if (processes < pipedProcesses.size() - 1)
            {
                if (dup2(pipefds[j + 1], 0) < 0)
                {
                    perror("Error with dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if (processes == pipedProcesses.size() - 1)
            {
                // Handle file output for the final process
                handleIO(pipedProcesses[processes]);
            }

            // If this isn't the first command and j != 2 * numPipes
            if (j != 0)
            {
                if (dup2(pipefds[j - 2], 0) < 0)
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

        j += 2;
    }

    for(i = 0; i < 2 * numPipes; i++)
    {
        close(pipefds[i]);
    }

    for(i = 0; i < numPipes + 1; i++)
    {
        wait(&status);
        return 0;
    }
}

int8_t Executor::executeForegroundProcess(Process foregroundProcess)
{
    pid_t pid = fork();

    // Check to make sure the process was correctly forked
    if (pid < 0)
    {
        std::cerr << "\tERROR: Failed to create new process" << std::endl;
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
            std::cerr << "\tERROR: Child process produced an error" << std::endl;
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
        std::cerr << "\tERROR: Failed to create new process" << std::endl;
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
    if (process.getCommand().compare("cd") == 0)
    {
        std::vector<std::string> args = process.getArgs();
        // Since we know it's only one argument which is the path to change the directory to,
        // we can just grab that sole argument

        if (args.size() == 0)
        {
            // We didn't have any arguments, so just print out the current directory
            char* directory = (char*) malloc(256);
            getcwd(directory, 256);
            std::cout << directory << std::endl;
            return 0;
        }


        int result = chdir(args[0].c_str());

        if (result < 0)
        {
            // We could not change the directory, so throw an error
            std::cerr << "\tERROR: No such file or directory" << std::endl;
        }
        else
        {
            // Changing directory was successful, print new directory
            char* newDirectory = (char*) malloc(256);
            getcwd(newDirectory, 256);
            std::cout << newDirectory << std::endl;
        }
        
    }
    else if (process.getCommand().compare("clr") == 0)
    {
        execlp("clear", "clear", NULL);
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
            cstrings.push_back(&string.front());
        }

        execvp("ls", cstrings.data());
    }
    else if (process.getCommand().compare("environ") == 0)
    {
        execlp("printenv", "printenv", NULL);
    }
    // else if (process.getCommand().compare("echo") == 0)
    // {
    //     // TODO: Handle echo command
    // }
    else if (process.getCommand().compare("help") == 0)
    {
        // TODO: Change format to work with piping
        execlp("help", "help", NULL);
    }
    else
    {
        std::vector<std::string> args = process.getArgs();

        if (args.size() == 0)
        {
            // No need to go through all the copying process if it's just a solo command with no args
            execlp(process.getCommand().c_str(), process.getCommand().c_str(), NULL);
        }

        std::vector<char*> cstrings{};
        // // Make sure to push back the command first
        // char* command = new char[process.getCommand().size() + 1];
        // std::copy(process.getCommand().begin(), process.getCommand().end(), command);

        // strcpy(command, process.getCommand().c_str());

        // cstrings.push_back(command);
        
        // Needed for a conversion from a vector of strings to a char** for execvp
        for(auto& string: args)
        {
            cstrings.push_back(&string.front());
        }

        // Make sure we push back a NULL to satisfy arguments for the linux comands
        cstrings.push_back(NULL);

        // Execute the process
        execvp(process.getCommand().c_str(), cstrings.data());   
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
            std::cerr << "\tERROR: Failed to open file: " + process.getInputFile() + " ABORTING..." << std::endl;
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
        // TODO: Change for depending on truncation or appending
        int file = open(process.getOuptutFile().c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        // Check to make sure the file was opened properly
        if (file < 0)
        {
            std::cerr << "\tERROR: Failed to open file: " + process.getInputFile() + " ABORTING..." << std::endl;
            return -1;
        }

        else
        {
            // Change the stdout to this file
            dup2(file, STDOUT_FILENO);

            // Close the file
            close(file);
        } 
    }

    return 0;
}