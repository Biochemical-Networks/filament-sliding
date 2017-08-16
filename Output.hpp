#ifndef OUPUT_HPP
#define OUPUT_HPP

#include <fstream>
#include <cstdint>
#include "SystemState.hpp"

class Output
{
private:
    std::ofstream m_microtubulePositionFile;
    const int32_t m_probePeriod; // Holds the number of time steps that are between two probes of the system, meaning that data will be written every so many time steps

    const int m_collumnWidth = 40;
public:
    Output(const std::string &runName, const int32_t probePeriod);
    ~Output();

    void writeMicrotubulePosition(const double time, const SystemState& systemState);

    int32_t getProbePeriod() const;
};

#endif // OUPUT_HPP
