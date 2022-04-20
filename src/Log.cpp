#include <fstream>
#include <iostream>
#include <string>

#include "filament-sliding/version.hpp"
#include "filament-sliding/Log.hpp"

Log::Log(const std::string& runName, const Clock& clock):
        m_clock(clock), m_logFile((runName + ".log.txt").c_str()) {
    m_logFile << "The git hash of the commit that was used to create the "
                 "current program is:\n"
              << GIT_COMMIT << "\n\n";

    // redirect messages and errors to file
    std::cout.rdbuf(m_logFile.rdbuf());
    std::cerr.rdbuf(m_logFile.rdbuf());
}

Log::~Log() {
    m_logFile << "\nExecution time: " << m_clock.now() << " seconds\n";
}

void Log::writeBoundaryProtocolAppearance(
        const int32_t numberDeterministic,
        const int32_t numberStochastic) {
    m_logFile << "\nThe deterministic boundary protocol was invoked "
              << numberDeterministic
              << ((numberDeterministic == 1) ? (" time.\n") : (" times.\n"));
    m_logFile << "The stochastic boundary protocol was invoked "
              << numberStochastic
              << ((numberStochastic == 1) ? (" time.\n") : (" times.\n"));
}
