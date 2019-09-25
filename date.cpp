#include "date.h"
#include <chrono>
#include <ctime>
#include <iostream>

int32_t Date::execute(std::vector<std::string>& outputVector)
{
    auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    // std::cout << ctime(&timenow) << std::endl << std::flush;
    outputVector.push_back(ctime(&timenow));
    return 0;
}