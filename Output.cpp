#include "Output.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip> // For std::setw()

#include "SystemState.hpp"
#include "OutputParameters.hpp"
#include "Histogram.hpp"

Output::Output(const std::string &runName,
               const bool writePositionalDistribution,
               const double positionalHistogramBinSize,
               const double positionalHistogramLowestValue,
               const double positionalHistogramHighestValue)
    :   m_microtubulePositionFile((runName+".microtubule_position.txt").c_str()),
        m_barrierCrossingTimeFile((runName+".times_barrier_crossings.txt").c_str()),
        m_statisticalAnalysisFile((runName+".statistical_analysis.txt").c_str()),
        m_collumnWidth(OutputParameters::collumnWidth),
        m_lastCrossingTime(0), // Time 0 indicates the beginning of the run blocks, after which we start writing data
        m_writePositionalDistribution(writePositionalDistribution),
        m_positionalHistogram(positionalHistogramBinSize, positionalHistogramLowestValue, positionalHistogramHighestValue)
{
    m_microtubulePositionFile << std::left
        << std::setw(m_collumnWidth) << "TIME"
        << std::setw(m_collumnWidth) << "POSITION" << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth

    m_barrierCrossingTimeFile << std::left
        << std::setw(m_collumnWidth) << "TIME CROSSING"
        << std::setw(m_collumnWidth) << "INTERVAL BETWEEN CROSSES"  << '\n'; // The '\n' needs to be separated, otherwise it will take one position from the collumnWidth

    m_statisticalAnalysisFile << std::left
        << std::setw(m_collumnWidth) << "RANDOM VARIABLE"
        << std::setw(m_collumnWidth) << "NUMBER OF SAMPLES"
        << std::setw(m_collumnWidth) << "MEAN"
        << std::setw(m_collumnWidth) << "VARIANCE"
        << std::setw(m_collumnWidth) << "STANDARD ERROR OF THE MEAN" << '\n';

    if(m_writePositionalDistribution)
    {
        // Only open the file here, not in the constructor initialization list, since the file should only be created if m_writePositionalDistribution is set to true.
        m_positionalHistogramFile.open((runName+".positional_histogram.txt").c_str());
        m_statisticalAnalysisFile << std::left
            << std::setw(m_collumnWidth) << "BIN BOUNDS"
            << std::setw(m_collumnWidth) << "NUMBER OF SAMPLES"
            << std::setw(m_collumnWidth) << "FRACTION OF SAMPLES" << '\n';
    }
}

Output::~Output()
{
    finishWriting();
}

void Output::writeMicrotubulePosition(const double time, const SystemState& systemState) // Non-const, stream is changed
{
    m_microtubulePositionFile << std::setw(m_collumnWidth) << time << std::setw(m_collumnWidth) << systemState.getMicrotubulePosition() << '\n';
}

void Output::addMicrotubulePositionRemainder(const double remainder)
{
    m_positionalHistogram.addValue(remainder);
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

void Output::finishWriting()
{
    m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "BARRIER CROSSING TIME"
        << std::setw(m_collumnWidth) << m_crossingTimeStatistics.getNumberOfSamples()
        << std::setw(m_collumnWidth) << m_crossingTimeStatistics.getMean()
        << std::setw(m_collumnWidth) << m_crossingTimeStatistics.getVariance()
        << std::setw(m_collumnWidth) << m_crossingTimeStatistics.getSEM() << '\n';

    if(m_writePositionalDistribution)
    {
        m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "REMAINDER TOP MICROTUBULE POSITION"
            << std::setw(m_collumnWidth) << m_positionalHistogram.getNumberOfSamples()
            << std::setw(m_collumnWidth) << m_positionalHistogram.getMean()
            << std::setw(m_collumnWidth) << m_positionalHistogram.getVariance()
            << std::setw(m_collumnWidth) << m_positionalHistogram.getSEM() << '\n';

        m_positionalHistogramFile << m_positionalHistogram;
    }
}
