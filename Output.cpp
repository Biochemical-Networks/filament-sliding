#include "Output.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip> // For std::setw()

#include "SystemState.hpp"

Output::Output(const std::string &runName)
    :   m_microtubulePositionFile((runName+".microtubule_position.txt").c_str()),
        m_barrierCrossingTimeFile((runName+".times_barrier_crossings.txt").c_str()),
        m_lastCrossingTime(0) // Time 0 indicates the beginning of the run blocks, after which we start writing data
{
    m_microtubulePositionFile << std::setw(m_collumnWidth) << "TIME"
        << std::setw(m_collumnWidth) << "POSITION" << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth

    m_barrierCrossingTimeFile << std::setw(m_collumnWidth) << "TIME CROSSING"
        << std::setw(m_collumnWidth) << "INTERVAL BETWEEN CROSSES"  << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth
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
    const double interval = time - m_lastCrossingTime;
    m_lastCrossingTime = time;
    m_barrierCrossingTimeFile << std::setw(m_collumnWidth) << time << std::setw(m_collumnWidth) << interval << '\n';

    m_crossingTimeStatistics.addValue(interval);
}

void Output::newBlock(const int32_t blockNumber)
{
    // make a single message that can be passed to all output files
    // std::ostringstream::ate (at end) makes sure that << appends new parts at the end of the message
    std::ostringstream message(std::ostringstream::ate);
    message.str("A new block starts. This has block number: ");
    message << blockNumber << '\n';
    m_microtubulePositionFile << message.str();
    m_barrierCrossingTimeFile << message.str();
}
