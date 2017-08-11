#include "Output.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip> // For std::setw()

Output::Output(const std::string &runName)
    :   m_microtubulePositionFile((runName+".microtubule_position.txt").c_str())
{
    m_microtubulePositionFile << std::setw(m_collumnWidth) << "TIME"
        << std::setw(m_collumnWidth) << "POSITION" << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth
}

Output::~Output()
{
}

void Output::writeMicrotubulePosition(const double time, const double position)
{
    m_microtubulePositionFile << std::setw(m_collumnWidth) << time << std::setw(m_collumnWidth) << position << '\n';
}
