#include "Log.hpp"
#include "GitHashWhenCompiled.hpp"

#include <fstream>
#include <string>
#include <iostream>

Log::Log(const std::string &runName, const Clock& clock)
    :   m_clock(clock),
        m_logFile((runName+".log.txt").c_str())
{
    m_logFile << "The git hash of the commit that was used to create the current program is:\n"
              << gitHash << "\n\n";

    // redirect messages and errors to file
    std::cout.rdbuf(m_logFile.rdbuf());
    std::cerr.rdbuf(m_logFile.rdbuf());
}

Log::~Log()
{
    m_logFile << "\nExecution time: " << m_clock.now() << " seconds\n";
}

void Log::writeBoundaryProtocolAppearance(const int32_t number)
{
    m_logFile << "\nThe deterministic boundary protocol was invoked " << number << ((number==1)?(" time.\n"):(" times.\n"));
}
