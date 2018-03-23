#include "Output.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip> // For std::setw()

#include "SystemState.hpp"

Output::Output(const std::string &runName)
    :   m_microtubulePositionFile((runName+".microtubule_position.txt").c_str()),
        m_barrierCrossingTimeFile((runName+".times_barrier_crossings.txt").c_str())
{
    m_microtubulePositionFile << std::setw(m_collumnWidth) << "TIME"
        << std::setw(m_collumnWidth) << "POSITION" << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth

    m_barrierCrossingTimeFile << std::setw(m_collumnWidth) << "TIME CROSSING" << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth
}

Output::~Output()
{
}

void Output::writeMicrotubulePosition(const double time, const SystemState& systemState) // Non-const, stream is changed
{
    m_microtubulePositionFile << std::setw(m_collumnWidth) << time << std::setw(m_collumnWidth) << systemState.getMicrotubulePosition() << '\n';
}

void Output::writeBarrierCrossingTime(const double time)
{
    m_barrierCrossingTimeFile << std::setw(m_collumnWidth) << time << '\n';
}
