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
#include "TransitionPath.hpp"

/* Output contains all output files, and contains functions to output information to those.
 * Further, it can do some data analysis. The results of this should then also be stored in a file.
 */

class Output
{
private:
    std::ofstream m_microtubulePositionFile;
    std::ofstream m_barrierCrossingTimeFile;
    std::ofstream m_positionalHistogramFile;
    std::ofstream m_positionAndConfigurationHistogramFile;
    std::ofstream m_transitionPathFile;
    std::ofstream m_transitionPathHistogramFile;
    std::ofstream m_statisticalAnalysisFile;

    const int m_collumnWidth;

    // Gather statistics:
    Statistics m_crossingTimeStatistics;
    double m_lastCrossingTime; // Necessary, since we want statistics on the time interval between the previous and next crossing

    const bool m_writePositionalDistribution;
    const bool m_recordTransitionPaths;

    int32_t m_nWrittenTransitionPaths;
    TransitionPath m_currentTransitionPath;
    bool m_isTrackingPath;

    std::unique_ptr<Histogram> mp_positionalHistogram;
    std::vector<Histogram> m_positionAndConfigurationHistogram;
    std::vector<Histogram> m_transitionPathHistogram;

public:
    Output(const std::string &runName,
           const bool writePositionalDistribution,
           const bool recordTransitionPaths,
           const int32_t transitionPathWriteFrequency,
           const double positionalHistogramBinSize,
           const double positionalHistogramLowestValue,
           const double positionalHistogramHighestValue,
           const int32_t maxNFullCrosslinkers);
    ~Output();

    void newBlock(const int32_t blockNumber);

    void writeMicrotubulePosition(const double time, const SystemState& systemState);

    void addMicrotubulePositionRemainder(const double remainder);

    void addPositionAndConfiguration(const double remainder, const int32_t nRightPullingCrosslinkers);

    void writeBarrierCrossingTime(const double time);

    void addPointTransitionPath(const double time, const double mobilePosition, const int32_t nRightPullingCrosslinkers);

    void addPositionAndConfigurationTransitionPath(const double remainder, const int32_t nRightPullingCrosslinkers);

    void writeTransitionPath(const double latticeSpacing);

    bool isTrackingPath() const;

    void toggleTracking();

    void cleanTransitionPath();

    void finishWriting();
};

#endif // OUPUT_HPP
