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
#include "parser.h"

#include <dirent.h>

int main(int argc, char *argv[])
{
    bool isRunning = true;

    // Clear the screen before printing out the program
    // TODO: Place this in a separate function
    std::cout << "\033[2J\033[1;1H";
    Executor executor = Executor();
    Parser parser = Parser();

    while (isRunning)
    {
        std::cout << getcwd(NULL, 0)  << "$ " << std::flush;
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
            std::vector<std::vector<Process>> processVector = parser.parseInput(input);
            executor.executeBatchProcess(processVector);
        }
    }

    // Clear the screen before exiting the program
    std::cout << "\033[2J\033[1;1H";
}