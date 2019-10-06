//
// Created by Benjamin Vogel on 9/16/2019.
//
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <fcntl.h>
#include "executor.h"
#include "parser.h"

/**
 * This file contains the main shell method
 **/

int main(int argc, char *argv[])
{
    bool isRunning = true;

    // Clear the screen before printing out the program
    std::cout << "\033[2J\033[1;1H";

    // Create new Executor and Parser objects
    Executor executor = Executor();
    Parser parser = Parser();

    if (argc == 1)
    {
        // There is no file to read from, so just act as a normal shell
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
                std::getline(std::cin, input);
            }
            else
            {
                std::vector<std::vector<Process>> processVector = parser.parseInput(input);
                executor.executeBatchProcess(processVector);
            }
        }
    }
    else if (argc == 2)
    {
        // We have a file to read from
        std::ifstream file;
        file.open(argv[1]);
        std::string line;
        if (!file.is_open())
        {
            perror("Error opening file");
            return -1;
        }

        while (std::getline(file, line))
        {
            if (line.compare("quit") == 0)
            {
                isRunning = false;
            }
            else
            {
                std::vector<std::vector<Process>> processVector = parser.parseInput(line);
                executor.executeBatchProcess(processVector);
            }
        }
    }
}