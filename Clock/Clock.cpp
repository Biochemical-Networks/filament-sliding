#include "Clock.hpp"
#include <chrono>
#include <ctime> // For printing the time
#include <iostream>
#include <iomanip>

// Initialise and store the time at construction, because for run intervals we will need to subtract the initial time
Clock::Clock() : m_constructionTime(std::chrono::steady_clock::now())
{
}

double Clock::now() const
{
    std::chrono::steady_clock::time_point currentTime{std::chrono::steady_clock::now()};
    double elapsedTime = std::chrono::duration<double>(currentTime - m_constructionTime).count(); // Counts seconds by default

    return elapsedTime;
}

Clock::~Clock()
{
    std::cout << "\nExecution time: " << now() << " seconds\n";
}

std::ostream& operator<< (std::ostream &out, const Clock &clock)
{
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    out << std::put_time(std::localtime(&currentTime), "%c %Z");
    return out;
}
