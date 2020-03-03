#ifndef OUPUT_HPP
#define OUPUT_HPP

#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <utility>

#include "SystemState.hpp"
#include "Statistics.hpp"
#include "Histogram.hpp"
#include "ActinDynamicsEstimate.hpp"
#include "ActinDisconnectException.hpp"
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

    // store the times of disconnecting in a vector the size of numberOfRuns.
    // Don't push back, since concurrent accessing could lead to data races.
    // Use the bool as a flag to indicate whether the value was set.
    std::vector<std::pair<bool,ActinDisconnectException>> m_disconnectTimes;

    Statistics m_actinTimeToDisconnectStatistics;
    Statistics m_actinPositionOfDisconnectStatistics;
    Statistics m_actinTimeLastTrackingCompletionStatistics;
    Statistics m_actinTotalTimeBehindTipStatistics;
    Statistics m_actinTotalTimeOnTipStatistics;
    Statistics m_actinBindProbabilityStatistics;

    void createActinDisconnectStatistics();

public:
    Output(const std::string &runName,
           const bool writePositionalDistribution,
           const double positionalHistogramBinSize,
           const double positionalHistogramLowestValue,
           const double positionalHistogramHighestValue,
           const double maxPeriodPositionTracking,
           const bool writeActinDynamicsEstimate,
           const int32_t numberOfRuns);

    ~Output();

    void writePositionsAndCrosslinkerNumbers(const double time, const SystemState& systemState);

    void addPosition(const double remainder);

    void addActinDisconnectTime(const int32_t runID, ActinDisconnectException&& disconnectInformation);

    void addActinBindingState(const bool actinIsBound);

    void finishWriting(const ActinDynamicsEstimate& completeActinDynamicsEstimate);
};

#endif // OUPUT_HPP
