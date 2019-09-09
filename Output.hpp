#ifndef OUPUT_HPP
#define OUPUT_HPP

#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>

#include "SystemState.hpp"
#include "Statistics.hpp"
#include "Histogram.hpp"
#include "ActinDynamicsEstimate.hpp"
/*#include "TransitionPath.hpp"*/

/* Output contains all output files, and contains functions to output information to those.
 * Further, it can do some data analysis. The results of this should then also be stored in a file.
 */

class Output
{
private:
    std::ofstream m_positionsAndCrosslinkersFile;
    std::ofstream m_positionalHistogramFile;
    std::ofstream m_statisticalAnalysisFile;
    std::ofstream m_actinDynamicsFile;

    const int m_collumnWidth;

    const bool m_writePositionalDistribution;

    std::unique_ptr<Histogram> mp_positionalHistogram;

    const double m_maxPeriodPositionTracking;

    const bool m_writeActinDynamicsEstimate;
    const ActinDynamicsEstimate& m_dynamicsEstimate;

public:
    Output(const std::string &runName,
           const bool writePositionalDistribution,
           const double positionalHistogramBinSize,
           const double positionalHistogramLowestValue,
           const double positionalHistogramHighestValue,
           const double maxPeriodPositionTracking,
           const bool writeActinDynamicsEstimate,
           const ActinDynamicsEstimate& dynamicsEstimate);

    ~Output();

    void writePositionsAndCrosslinkerNumbers(const double time, const SystemState& systemState);

    void addPosition(const double remainder);

    void finishWriting();
};

#endif // OUPUT_HPP
