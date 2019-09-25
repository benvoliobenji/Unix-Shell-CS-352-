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
#include "date.h"

int main(int argc, char *argv[])
{
    bool isRunning = true;

    // Clear the screen before printing out the program
    // TODO: Place this in a separate function
    std::cout << "\033[2J\033[1;1H";

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
        else if (input.compare("clr") == 0)
        {
            // A special command to clear the screen that works for Linux and Windows
            // Also avoids using system() calls
            std::cout << "\033[2J\033[1;1H";
        }
        else if (input.compare("date") == 0)
        {
            pid_t datePID = fork();

            if (datePID == 0)
            {
                // Currently only treated as a foreground process, eventually add support for background process
                // Also, do execlp? Problem is no direct vector support
                int32_t jobNumber = 1;
                Date dateProcess = Date(datePID, jobNumber);
                std::vector<std::string> output;
                int32_t result = dateProcess.execute(output);
                if (result == 0)
                {
                    auto dateIterator = output.begin();
                    for (; dateIterator != output.end(); ++dateIterator)
                    {
                        std::cout << *dateIterator << std::flush;
                    }
                    exit(0);
                }
            }
            else 
            {
                wait(NULL);
            }
        }
    }

    // Clear the screen before exiting the program
    std::cout << "\033[2J\033[1;1H";
}