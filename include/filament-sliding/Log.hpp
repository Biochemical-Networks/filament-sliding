#ifndef LOG_HPP
#define LOG_HPP

#include <fstream>
#include <string>

#include "filament-sliding/Clock.hpp"

class Log {
  private:
    const Clock& m_clock;
    std::ofstream m_logFile;

  public:
    Log(const std::string& runName, const Clock& clock);
    ~Log();

    void writeBoundaryProtocolAppearance(
            const int32_t numberDeterministic,
            const int32_t numberStochastic);
};

#endif // LOG_HPP
