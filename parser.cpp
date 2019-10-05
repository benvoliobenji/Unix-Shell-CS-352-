#include <sstream>
#include <iostream>
#include <unistd.h>
#include "trim.h"
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
        token = trim(token);
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
        token = trim(token);
        pipedProcessStringVector.push_back(token);
    }

    return pipedProcessStringVector;
}

Process Parser::parseArguments(std::string processArgs)
{
    // Cut up the string line into a vector of individual string arguments
    std::stringstream ss(processArgs);
    std::vector<std::string> argVector;
    std::string token;

    while (std::getline(ss, token, ' '))
    {
        argVector.push_back(token);
    }

    // The first argument will be the command
    std::string command = argVector[0];

    // Values that will be needed to be held for setting the process values
    bool isBackground = false;
    std::vector<std::string> argumentVector;
    std::string inputFile = "";
    std::string outputFile = "";
    bool hasInputFileArg = false;
    bool hasOutputFileArg = false;
    bool isTruncated = true;

    for(auto argVectorIterator = argVector.begin(); argVectorIterator != argVector.end(); ++argVectorIterator)
    {
        std::string arg = *argVectorIterator;
        if(arg.compare(" ") == 0)
        {
            // If it's an empty arg, just continue
            continue;
        }
        else if (arg.compare("&") == 0)
        {
            // This is the argument needed to determine if it's a background process
            isBackground = true;
        }
        else if (arg.compare(">>") == 0)
        {
            // This means the output file must be appended, not truncated
            isTruncated = false;
            hasOutputFileArg = true;
        }
        else if (arg.compare(">") == 0)
        {
            isTruncated = true;
            hasOutputFileArg = true;
        }
        else if (arg.compare("<") == 0)
        {
            // This is the indicator that we have an input file argument
            hasInputFileArg = true;
        }
        else if (hasInputFileArg == true)
        {
            // The input file argument will always follow the < command
            inputFile = arg;
            hasInputFileArg = false;
        }
        else if (hasOutputFileArg == true)
        {
            // The output file argument will always follow the > or >> command
            outputFile = arg;
            hasOutputFileArg = false;
        }
        else
        {
            // Otherwise, we just have an argument
            argumentVector.push_back(arg);
        } 
    }

    // Create a new process and set it's values
    Process newProcess = Process(getpid(), command, argumentVector, isBackground);
    newProcess.setInputFile(inputFile);
    newProcess.setOutputFile(outputFile);
    newProcess.setOutputFileTruncated(isTruncated);

    return newProcess;
}