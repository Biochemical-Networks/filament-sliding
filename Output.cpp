#include "Output.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip> // For std::setw()
#include <memory> // std::unique_ptr
#include <utility> // std::move
#include <stdexcept>

#include "SystemState.hpp"
#include "OutputParameters.hpp"
#include "Histogram.hpp"
#include "MathematicalFunctions.hpp"

Output::Output(const std::string &runName,
               const bool writePositionalDistribution,
               const double positionalHistogramBinSize,
               const double positionalHistogramLowestValue,
               const double positionalHistogramHighestValue,
               const double maxPeriodPositionTracking)
    :   m_positionsAndCrosslinkersFile((runName+".filament_positions_and_crosslinker_numbers.txt").c_str()),
        m_statisticalAnalysisFile((runName+".statistical_analysis.txt").c_str()),
        m_collumnWidth(OutputParameters::collumnWidth),
        m_writePositionalDistribution(writePositionalDistribution),
        m_maxPeriodPositionTracking(maxPeriodPositionTracking)
{
    m_positionsAndCrosslinkersFile << std::left
        << std::setw(m_collumnWidth) << "TIME"
        << std::setw(m_collumnWidth) << "TIPFRONT"
        << std::setw(m_collumnWidth) << "TIPMEAN"
        << std::setw(m_collumnWidth) << "ACTPOS"
        << std::setw(m_collumnWidth) << "N"
        << std::setw(m_collumnWidth) << "NR" << '\n';

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
    }
}

Output::~Output()
{
    finishWriting();
}

void Output::writePositionsAndCrosslinkerNumbers(const double time, const SystemState& systemState)
{
    if(time <= m_maxPeriodPositionTracking)
    {
        m_positionsAndCrosslinkersFile << std::setw(m_collumnWidth) << time
            << std::setw(m_collumnWidth) << systemState.getNSites(MicrotubuleType::FIXED)*systemState.getLatticeSpacing()
            << std::setw(m_collumnWidth) << systemState.getPositionMicrotubuleTip()
            << std::setw(m_collumnWidth) << systemState.getMicrotubulePosition()
                +systemState.getNSites(MicrotubuleType::MOBILE)*systemState.getLatticeSpacing()
            << std::setw(m_collumnWidth) << systemState.getNFullCrosslinkers()
            << std::setw(m_collumnWidth) << systemState.getNFullRightPullingCrosslinkers() << '\n';
    }
}

void Output::addPosition(const double remainder)
{
    mp_positionalHistogram->addValue(remainder);
}

void Output::finishWriting()
{
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
    }
}
