#include "Clock.hpp"
#include <chrono>
#include <iostream>

// Initialise and store the time at construction, because for run intervals we will need to subtract the initial time
Clock::Clock() : m_constructionTime(std::chrono::steady_clock::now())
{
}

double Clock::now()
{
    std::chrono::steady_clock::time_point currentTime{std::chrono::steady_clock::now()};
    double elapsedTime = std::chrono::duration<double>(currentTime - m_constructionTime).count(); // Counts seconds by default

    return elapsedTime;
}

Clock::~Clock()
{
    std::cout << "\nExecution time: " << now() << " seconds\n";
}
