#ifndef OUPUT_HPP
#define OUPUT_HPP

#include <fstream>
#include <string>

#include "SystemState.hpp"

/* Output contains all output files, and contains functions to output information to those.
 * Further, it can do some data analysis. The results of this should then also be stored in a file.
 */

class Output
{
private:
    std::ofstream m_microtubulePositionFile;
    std::ofstream m_barrierCrossingTimeFile;

    const int m_collumnWidth = 40;
public:
    Output(const std::string &runName);
    ~Output();

    void newBlock(const int32_t blockNumber);

    void writeMicrotubulePosition(const double time, const SystemState& systemState);

    void writeBarrierCrossingTime(const double time);
};

#endif // OUPUT_HPP
