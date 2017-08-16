#include "Output.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip> // For std::setw()

#include "SystemState.hpp"

Output::Output(const std::string &runName, const int32_t probePeriod)
    :   m_microtubulePositionFile((runName+".microtubule_position.txt").c_str()),
        m_probePeriod(probePeriod)
{
    m_microtubulePositionFile << std::setw(m_collumnWidth) << "TIME"
        << std::setw(m_collumnWidth) << "POSITION" << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth
}

Output::~Output()
{
}

void Output::writeMicrotubulePosition(const double time, const SystemState& systemState) // Non-const, stream is changed
{
    m_microtubulePositionFile << std::setw(m_collumnWidth) << time << std::setw(m_collumnWidth) << systemState.getMicrotubulePosition() << '\n';
}

int32_t Output::getProbePeriod() const
{
    return m_probePeriod;
}
