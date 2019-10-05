#include <sstream>
#include <iostream>
#include "parser.h"

std::vector<std::vector<Process>> Parser::parseInput(std::string inputLine)
{
    std::vector<std::string> independentProcesses = parseIndependentProcesses(inputLine);

    std::vector<std::vector<Process>> processVector = {};

    for (auto independentProcessesIterator = independentProcesses.begin();
            independentProcessesIterator != independentProcesses.end(); ++independentProcessesIterator)
    {
        std::vector<std::string> pipedStringProcesses = parsePipedProcesses(*independentProcessesIterator);

        std::vector<Process> pipedProcesses = {};

        for(auto pipedProcessesIterator = pipedStringProcesses.begin(); pipedProcessesIterator != pipedStringProcesses.end(); ++pipedProcessesIterator)
        {
            Process process = parseArguments(*pipedProcessesIterator);
            
            pipedProcesses.push_back(process);
        }

        processVector.push_back(pipedProcesses);
    }

    return processVector;
}

std::vector<std::string> Parser::parseIndependentProcesses(std::string inputLine)
{
    std::stringstream ss(inputLine);
    std::vector<std::string> independentProcessStringVector;

    std::string token;

    // This is the ASCII character representation for ";" since ";" is a char[2]
    char delim = putchar(59);

    while (std::getline(ss, token, delim))
    {
        independentProcessStringVector.push_back(token);
    }

    return independentProcessStringVector;
}