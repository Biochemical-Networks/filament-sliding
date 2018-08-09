#include "Output.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip> // For std::setw()
#include <memory> // std::unique_ptr
#include <utility> // std::move

#include "SystemState.hpp"
#include "OutputParameters.hpp"
#include "Histogram.hpp"
#include "MathematicalFunctions.hpp"

Output::Output(const std::string &runName,
               const bool writePositionalDistribution,
               const bool recordTransitionPaths,
               const int32_t transitionPathWriteFrequency,
               const int32_t maxNumberTransitionPaths,
               const double positionalHistogramBinSize,
               const double positionalHistogramLowestValue,
               const double positionalHistogramHighestValue,
               const int32_t maxNFullCrosslinkers,
               const double maxPeriodPositionTracking)
    :   m_microtubulePositionFile((runName+".microtubule_position.txt").c_str()),
        m_barrierCrossingTimeFile((runName+".times_barrier_crossings.txt").c_str()),
        m_statisticalAnalysisFile((runName+".statistical_analysis.txt").c_str()),
        m_collumnWidth(OutputParameters::collumnWidth),
        m_lastCrossingTime(0), // Time 0 indicates the beginning of the run blocks, after which we start writing data
        m_writePositionalDistribution(writePositionalDistribution),
        m_recordTransitionPaths(recordTransitionPaths),
        m_maxNumberTransitionPaths(maxNumberTransitionPaths),
        m_nWrittenTransitionPaths(0),
        m_currentTransitionPath(transitionPathWriteFrequency),
        m_isTrackingPath(false),
        m_maxNFullCrosslinkers(maxNFullCrosslinkers),
        m_maxPeriodPositionTracking(maxPeriodPositionTracking)
{
    m_microtubulePositionFile << std::left
        << std::setw(m_collumnWidth) << "TIME"
        << std::setw(m_collumnWidth) << "POSITION"
        << std::setw(m_collumnWidth) << "NUMBER RIGHT PULLING LINKERS" << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth

    m_barrierCrossingTimeFile << std::left
        << std::setw(m_collumnWidth) << "TIME CROSSING"
        << std::setw(m_collumnWidth) << "INTERVAL BETWEEN CROSSES"
        << std::setw(m_collumnWidth) << "HOP DIRECTION" << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth

    m_statisticalAnalysisFile << std::left
        << std::setw(m_collumnWidth) << "RANDOM VARIABLE"
        << std::setw(m_collumnWidth) << "NUMBER OF SAMPLES"
        << std::setw(m_collumnWidth) << "MEAN"
        << std::setw(m_collumnWidth) << "VARIANCE"
        << std::setw(m_collumnWidth) << "STANDARD ERROR OF THE MEAN" << '\n';

    if(m_writePositionalDistribution)
    {
        mp_positionalHistogram = std::move(std::unique_ptr<Histogram>(new Histogram(positionalHistogramBinSize, positionalHistogramLowestValue, positionalHistogramHighestValue)));
        // Only open the file here, not in the constructor initialization list, since the file should only be created if m_writePositionalDistribution is set to true.
        m_positionalHistogramFile.open((runName+".positional_histogram.txt").c_str());
        m_positionalHistogramFile << std::left
            << std::setw(m_collumnWidth) << "LOWER BIN BOUND"
            << std::setw(m_collumnWidth) << "UPPER BIN BOUND"
            << std::setw(m_collumnWidth) << "NUMBER OF SAMPLES"
            << std::setw(m_collumnWidth) << "FRACTION OF SAMPLES" << '\n';

        // nR is the number of right pulling linkers
        for(int32_t nR = 0; nR <= maxNFullCrosslinkers; ++nR) // nR can equal maxNFullCrosslinkers!
        {
            m_positionAndConfigurationHistogram.push_back(Histogram(positionalHistogramBinSize, positionalHistogramLowestValue, positionalHistogramHighestValue));
        }

        m_positionAndConfigurationHistogramFile.open((runName+".position_configuration_histogram.txt").c_str());
        m_positionAndConfigurationHistogramFile << std::left
            << std::setw(m_collumnWidth) << "LOWER BIN BOUND"
            << std::setw(m_collumnWidth) << "UPPER BIN BOUND"
            << std::setw(m_collumnWidth) << "NUMBER OF SAMPLES"
            << std::setw(m_collumnWidth) << "FRACTION OF SAMPLES GIVEN NR" << '\n';
    }

    if(m_recordTransitionPaths)
    {
        // Only open the file here, not in the constructor initialization list, since the file should only be created if m_recordTransitionPaths is set to true.
        m_transitionPathFile.open((runName+".transition_paths.txt").c_str());
        m_transitionPathFile << std::left
        << std::setw(m_collumnWidth) << "TIME"
        << std::setw(m_collumnWidth) << "POSITION"
        << std::setw(m_collumnWidth) << "NUMBER RIGHT PULLING LINKERS" << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth
    }

    if(m_writePositionalDistribution && m_recordTransitionPaths)
    {
        // nR is the number of right pulling linkers
        for(int32_t nR = 0; nR <= maxNFullCrosslinkers; ++nR) // nR can equal maxNFullCrosslinkers!
        {
            m_transitionPathHistogram.push_back(Histogram(positionalHistogramBinSize, positionalHistogramLowestValue, positionalHistogramHighestValue));
        }

        m_transitionPathHistogramFile.open((runName+".transition_path_histogram.txt").c_str());
        m_transitionPathHistogramFile << std::left
            << std::setw(m_collumnWidth) << "LOWER BIN BOUND"
            << std::setw(m_collumnWidth) << "UPPER BIN BOUND"
            << std::setw(m_collumnWidth) << "NUMBER OF SAMPLES"
            << std::setw(m_collumnWidth) << "FRACTION OF SAMPLES GIVEN NR" << '\n';
    }
}

Output::~Output()
{
    finishWriting();
}

void Output::writeMicrotubulePosition(const double time, const SystemState& systemState) // Non-const, stream is changed
{
    // make sure that the file does not bloat
    if(time <= m_maxPeriodPositionTracking)
    {
        m_microtubulePositionFile << std::setw(m_collumnWidth) << time
                                  << std::setw(m_collumnWidth) << systemState.getMicrotubulePosition()
                                  << std::setw(m_collumnWidth) << systemState.getNFullRightPullingCrosslinkers() << '\n';
    }
}

void Output::addMicrotubulePositionRemainder(const double remainder)
{
    mp_positionalHistogram->addValue(remainder);
}

void Output::addPositionAndConfiguration(const double remainder, const int32_t nRightPullingCrosslinkers)
{
    try
    {
        m_positionAndConfigurationHistogram.at(nRightPullingCrosslinkers).addValue(remainder);
    }
    catch(const std::out_of_range& outOfRange)
    {
        throw GeneralException(std::string("Output::addPositionAndConfiguration() was called with nRightPullingCrosslinkers out of range: ")+std::string(outOfRange.what()));
    }
}

void Output::writeBarrierCrossingTime(const double time, const int32_t direction)
{
    const double interval = time - m_lastCrossingTime;
    m_lastCrossingTime = time;
    m_barrierCrossingTimeFile << std::setw(m_collumnWidth) << time << std::setw(m_collumnWidth) << interval << std::setw(m_collumnWidth) << direction << '\n';

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

void Output::addPointTransitionPath(const double time, const double mobilePosition, const int32_t nRightPullingCrosslinkers)
{
    #ifdef MYDEBUG
    if(!m_isTrackingPath)
    {
        throw GeneralException("Output::addPointTransitionPath() was called, but m_isTrackingPath is set to FALSE.");
    }
    #endif // MYDEBUG

    m_currentTransitionPath.addPoint(time, mobilePosition, nRightPullingCrosslinkers);
}

void Output::addPositionAndConfigurationTransitionPath(const double remainder, const int32_t nRightPullingCrosslinkers)
{
    #ifdef MYDEBUG
    if(!m_isTrackingPath || !m_recordTransitionPaths || !m_writePositionalDistribution)
    {
        throw GeneralException("Output::addPositionAndConfigurationTransitionPath() is called in improper circumstances");
    }
    #endif // MYDEBUG

    try
    {
        m_transitionPathHistogram.at(nRightPullingCrosslinkers).addValue(remainder);
    }
    catch(const std::out_of_range& outOfRange)
    {
        throw GeneralException(std::string("Output::addPositionAndConfigurationTransitionPath() was called with nRightPullingCrosslinkers out of range: ")+std::string(outOfRange.what()));
    }
}

void Output::writeTransitionPath(const double latticeSpacing)
{
    #ifdef MYDEBUG
    if(!m_recordTransitionPaths)
    {
        throw GeneralException("Output::writeTransitionPath() was called, but recordTransitionPaths is set to FALSE.");
    }
    #endif // MYDEBUG

    // Does the current transition path move forward in direction (true) or backward (false)?
    const bool isForwardPath = (m_currentTransitionPath.getMobilePosition(m_currentTransitionPath.getSize()-1) > m_currentTransitionPath.getMobilePosition(0));

    for(int32_t label=0; label<m_currentTransitionPath.getSize(); ++label)
    {
        // Flip each backward path to a forward path, with x -> d-x, nR -> n-nR.
        // This way, the histogram represents the statistics of one directional paths.
        // Since the probability of a path should not depend on its direction in time, this should actually not change the histogram.
        double x = m_currentTransitionPath.getMobilePosition(label);
        x = isForwardPath?x:latticeSpacing-x;
        x = MathematicalFunctions::mod(x, latticeSpacing);
        int32_t nR = m_currentTransitionPath.getNRightPullingLinkers(label);
        nR = isForwardPath?nR:m_maxNFullCrosslinkers-nR;

        #ifdef MYDEBUG
        if(nR<0 || nR > m_maxNFullCrosslinkers)
        {
            throw GeneralException("Output::writeTransitionPath() encountered a wrong number of right pulling crosslinkers.");
        }
        #endif // MYDEBUG

        addPositionAndConfigurationTransitionPath(x, nR);
    }

    if(m_nWrittenTransitionPaths < m_maxNumberTransitionPaths) // Limit the size of m_transitionPathFile
    {
        m_transitionPathFile << "New transition path; path number: " << m_nWrittenTransitionPaths << '\n';
        m_transitionPathFile << m_currentTransitionPath;
    }
    m_currentTransitionPath.clean();
    ++m_nWrittenTransitionPaths;
}

bool Output::isTrackingPath() const
{
    return m_isTrackingPath;
}

void Output::toggleTracking()
{
    m_isTrackingPath = !m_isTrackingPath;
}

void Output::cleanTransitionPath()
{
    m_currentTransitionPath.clean();
}

void Output::finishWriting()
{
    if(m_crossingTimeStatistics.canReportStatistics())
    {
        m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "BARRIER CROSSING TIME"
            << std::setw(m_collumnWidth) << m_crossingTimeStatistics.getNumberOfSamples()
            << std::setw(m_collumnWidth) << m_crossingTimeStatistics.getMean()
            << std::setw(m_collumnWidth) << m_crossingTimeStatistics.getVariance()
            << std::setw(m_collumnWidth) << m_crossingTimeStatistics.getSEM() << '\n';
    }

    if(m_writePositionalDistribution)
    {
        if(mp_positionalHistogram->canReportStatistics())
        {
            m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "REMAINDER TOP MICROTUBULE POSITION"
                << std::setw(m_collumnWidth) << mp_positionalHistogram->getNumberOfSamples()
                << std::setw(m_collumnWidth) << mp_positionalHistogram->getMean()
                << std::setw(m_collumnWidth) << mp_positionalHistogram->getVariance()
                << std::setw(m_collumnWidth) << mp_positionalHistogram->getSEM() << '\n';

            m_positionalHistogramFile << (*mp_positionalHistogram);
        }

        // nR is the number of right pulling linkers
        // m_positionAndConfigurationHistogram.size() = maxNFullCrosslinkers + 1
        for(uint32_t nR = 0; nR < m_positionAndConfigurationHistogram.size(); ++nR)
        {
            if(m_positionAndConfigurationHistogram[nR].canReportStatistics())
            {
                m_statisticalAnalysisFile << std::setw(m_collumnWidth)
                    << (std::string("REMAINDER WITH NR=")+std::to_string(nR))
                    << std::setw(m_collumnWidth) << m_positionAndConfigurationHistogram[nR].getNumberOfSamples()
                    << std::setw(m_collumnWidth) << m_positionAndConfigurationHistogram[nR].getMean()
                    << std::setw(m_collumnWidth) << m_positionAndConfigurationHistogram[nR].getVariance()
                    << std::setw(m_collumnWidth) << m_positionAndConfigurationHistogram[nR].getSEM() << '\n';

                m_positionAndConfigurationHistogramFile << (std::string("HISTOGRAM FOR NR=")+std::to_string(nR)) << '\n';
                m_positionAndConfigurationHistogramFile << m_positionAndConfigurationHistogram[nR];
            }
        }
    }

    if(m_writePositionalDistribution && m_recordTransitionPaths)
    {
        // nR is the number of right pulling linkers
        // m_transitionPathHistogram.size() = maxNFullCrosslinkers + 1
        for(uint32_t nR = 0; nR < m_transitionPathHistogram.size(); ++nR)
        {
            if(m_transitionPathHistogram[nR].canReportStatistics())
            {
                m_statisticalAnalysisFile << std::setw(m_collumnWidth)
                    << (std::string("TRANSITION PATH REMAINDER WITH NR=")+std::to_string(nR))
                    << std::setw(m_collumnWidth) << m_transitionPathHistogram[nR].getNumberOfSamples()
                    << std::setw(m_collumnWidth) << m_transitionPathHistogram[nR].getMean()
                    << std::setw(m_collumnWidth) << m_transitionPathHistogram[nR].getVariance()
                    << std::setw(m_collumnWidth) << m_transitionPathHistogram[nR].getSEM() << '\n';

                m_transitionPathHistogramFile << (std::string("TRANSITION PATH HISTOGRAM FOR NR=")+std::to_string(nR)) << '\n';
                m_transitionPathHistogramFile << m_transitionPathHistogram[nR];
            }
        }
    }
}
