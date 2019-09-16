//
// Created by Benjamin Vogel on 9/16/2019.
//
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char *argv[])
{
    bool isRunning = true;

    while (isRunning)
    {
        std::cout << getcwd(NULL, 0) << "$ " << std::flush;
        std::string input;
        std::getline(input);

        if (input.compare("quit") == 0)
        {
            isRunning = false;
        }
    }
}