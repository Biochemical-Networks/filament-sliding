#ifndef LOG_HPP
#define LOG_HPP

#include "Clock/Clock.hpp"

#include <fstream>
#include <string>

class Log
{
private:
    const Clock& m_clock;
    std::ofstream m_logFile;
public:
    Log(const std::string &runName, const Clock& clock);
    ~Log();
};

#endif // LOG_HPP
