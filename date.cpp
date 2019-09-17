#include "date.h"
#include <chrono>
#include <ctime>
#include <iostream>

int32_t Date::execute()
{
    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
    std::cout << ctime(&timenow) << std::endl << std::flush;
    return 0;
}