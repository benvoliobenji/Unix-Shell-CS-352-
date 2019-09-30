//
// Created by Benjamin Vogel on 9/16/2019.
//
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include "executor.h"

#include <dirent.h>

int main(int argc, char *argv[])
{
    bool isRunning = true;

    // Clear the screen before printing out the program
    // TODO: Place this in a separate function
    std::cout << "\033[2J\033[1;1H";
    Executor executor = Executor();

    while (isRunning)
    {
        std::cout << getcwd(NULL, 0) << "$ " << std::flush;
        std::string input;
        std::getline(std::cin, input);

        if (input.compare("quit") == 0)
        {
            isRunning = false;
        }
        else if (input.compare("pause") == 0)
        {
            std::cout << "Press ENTER to Continue: ";
            fgetc(stdin);
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else
        {
            std::vector<Process> processVector;
            std::vector<std::string> args = {};
            Process newProcess = Process(getpid(), input, args, false);

            processVector.push_back(newProcess);
            executor.executeBatchProcess(processVector);
        }
        
        // else if (input.compare("clr") == 0)
        // {
        //     pid_t forkPID = fork();

        //     if (forkPID == 0)
        //     {
        //         execlp("clear", "clear", NULL);
        //         return 0;
        //     }
        //     else
        //     {
        //         int status = 0;
        //         waitpid(forkPID, &status, 0);
        //     }
            
        // }
        // else if (input.compare("date") == 0)
        // {
        //     pid_t datePID = fork();

        //     if (datePID == 0)
        //     {
        //         // Currently only treated as a foreground process, eventually add support for background process
        //         // Also, do execlp? Problem is no direct vector support
        //         // int32_t jobNumber = 1;
        //         // Date dateProcess = Date(datePID, jobNumber);
        //         // std::vector<std::string> output;
        //         // int32_t result = dateProcess.execute(output);
        //         // if (result == 0)
        //         // {
        //         //     auto dateIterator = output.begin();
        //         //     for (; dateIterator != output.end(); ++dateIterator)
        //         //     {
        //         //         std::cout << *dateIterator << std::flush;
        //         //     }
        //         //     std::cout << std::endl;
        //         //     return 0;
        //         // }
        //         execlp("date", "date", 0);
        //     }
        //     else 
        //     {
        //         int status = 0;
        //         waitpid(datePID, &status, 0);
        //     }
        // }
        // else if (input.compare("cal") == 0)
        // {
        //     pid_t calPID = fork();
            
        //     if (calPID == 0)
        //     {
        //         execlp("cal", "cal", NULL);
        //         return 0;
        //     }
        //     else
        //     {
        //         int status = 0;
        //         waitpid(calPID, &status, 0);
        //     }      
        // }
        // else if (input.compare("ls") == 0)
        // {
        //     pid_t lsPID = fork();
        //     if (lsPID == 0)
        //     {
        //         // DIR *dir;
        //         // dirent *pdir;
        //         // dir = opendir(".");
        //         // while(readdir(dir))
        //         // {
        //         //     pdir = readdir(dir);
        //         //     std::cout << pdir->d_name << "\t";
        //         // }
        //         // closedir(dir);
        //         // return 0;
        //         execlp("ls", "ls", NULL);
        //         return 0;
        //     }
        //     else
        //     {
        //         int status = 0;
        //         waitpid(lsPID, &status, 0);
        //     }
        // }
    }

    // Clear the screen before exiting the program
    std::cout << "\033[2J\033[1;1H";
}