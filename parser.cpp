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

    while (std::getline(ss, token, ';'))
    {
        independentProcessStringVector.push_back(token);
    }

    return independentProcessStringVector;
}

std::vector<std::string> Parser::parsePipedProcesses(std::string independentProcess)
{
    std::stringstream ss(independentProcess);
    std::vector<std::string> pipedProcessStringVector;

    std::string token;

    while (std::getline(ss, token, '|'))
    {
        pipedProcessStringVector.push_back(token);
    }

    return pipedProcessStringVector;
}

Process Parser::parseArguments(std::string processArgs)
{
    std::stringstream ss(processArgs);
    std::vector<std::string> argVector;
    std::string token;

    while (std::getline(ss, token, ' '))
    {
        argVector.push_back(token);
    }

    std::string command = argVector[0];

    for(auto argVectorIterator = argVector.begin(); argVectorIterator != argVector.end(); ++argVectorIterator)
    {
        std::string arg = *argVectorIterator;
        if(arg.compare(" ") == 0)
        {
            continue;
        }
    }
}