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
#include "ActinDisconnectException.hpp"

Output::Output(const std::string &runName,
               const bool writePositionalDistribution,
               const double positionalHistogramBinSize,
               const double positionalHistogramLowestValue,
               const double positionalHistogramHighestValue,
               const double maxPeriodPositionTracking,
               const bool writeActinDynamicsEstimate,
               const int32_t numberOfRuns)
    :   m_positionsAndCrosslinkersFile((runName+".filament_positions_and_crosslinker_numbers.txt").c_str()),
        m_statisticalAnalysisFile((runName+".statistical_analysis.txt").c_str()),
        m_collumnWidth(OutputParameters::collumnWidth),
        m_writePositionalDistribution(writePositionalDistribution),
        m_maxPeriodPositionTracking(maxPeriodPositionTracking),
        m_writeActinDynamicsEstimate(writeActinDynamicsEstimate),
        m_disconnectTimes(numberOfRuns, std::make_pair(false, ActinDisconnectException{}))
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

    if(m_writeActinDynamicsEstimate)
    {
        m_actinDynamicsFile.open((runName+".actin_dynamics_estimates.txt").c_str());
        m_actinDynamicsFile << std::left
            << std::setw(m_collumnWidth) << "LOWER BIN BOUND"
            << std::setw(m_collumnWidth) << "UPPER BIN BOUND"
            << std::setw(m_collumnWidth) << "NUMBER OF SAMPLES"
            << std::setw(m_collumnWidth) << "DIFFUSION CONSTANT (micron^2/s)"
            << std::setw(m_collumnWidth) << "FORCE (kT/micron)" << '\n';
    }
}

Output::~Output()
{
}

void Output::writePositionsAndCrosslinkerNumbers(const double time, const SystemState& systemState)
{
    if(time <= m_maxPeriodPositionTracking)
    {
        m_positionsAndCrosslinkersFile << std::setw(m_collumnWidth) << time
            << std::setw(m_collumnWidth) << (systemState.getNSites(MicrotubuleType::FIXED)-1)*systemState.getLatticeSpacing()
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

void Output::addActinDisconnectTime(const int32_t runID, ActinDisconnectException&& disconnectInformation)
{
    #ifdef MYDEBUG
    if(runID<0 || runID>=static_cast<int32_t>(m_disconnectTimes.size()))
    {
        throw GeneralException("Output::addActinDisconnectTime() encountered a wrong runID.");
    }
    #endif // MYDEBUG
    m_disconnectTimes.at(runID) = std::make_pair(true, std::move(disconnectInformation));
}

void Output::addActinBindingState(const bool actinIsBound)
{
    m_actinBindProbabilityStatistics.addValue(actinIsBound?1.0:0.0);
}

void Output::finishWriting(const ActinDynamicsEstimate& completeActinDynamicsEstimate)
{
    if(m_writePositionalDistribution)
    {
        if(mp_positionalHistogram->canReportStatistics())
        {
            m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "REMAINDER TOP MICROTUBULE POSITION"
                << static_cast<Statistics>(*mp_positionalHistogram);

            m_positionalHistogramFile << (*mp_positionalHistogram);
        }
    }

    if(m_disconnectTimes.size()>1)
    {
        createActinDisconnectStatistics();
    }

    if(m_actinTimeToDisconnectStatistics.canReportStatistics())
    {
        m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "DISCONNECT TIMES" << m_actinTimeToDisconnectStatistics;
    }
    if(m_actinPositionOfDisconnectStatistics.canReportStatistics())
    {
        m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "DISCONNECT POSITIONS" << m_actinPositionOfDisconnectStatistics;
    }
    if(m_actinTimeLastTrackingCompletionStatistics.canReportStatistics())
    {
        m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "LAST TRACKING TIMES" << m_actinTimeLastTrackingCompletionStatistics;
    }
    if(m_actinTotalTimeBehindTipStatistics.canReportStatistics())
    {
        m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "TOTAL LATTICE TIME" << m_actinTotalTimeBehindTipStatistics;
    }
    if(m_actinTotalTimeOnTipStatistics.canReportStatistics())
    {
        m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "TOTAL TIP TIME" << m_actinTotalTimeOnTipStatistics;
    }

    if(m_actinBindProbabilityStatistics.canReportStatistics())
    {
        m_statisticalAnalysisFile << std::setw(m_collumnWidth) << "ACTIN BINDING PROBABILITY" << m_actinBindProbabilityStatistics;
    }

    if(m_writeActinDynamicsEstimate)
    {
        m_actinDynamicsFile << completeActinDynamicsEstimate;
    }
}

void Output::createActinDisconnectStatistics()
{
    #ifdef MYDEBUG
    if(m_disconnectTimes.size()<2)
    {
        throw GeneralException("Output::createActinDisconnectStatistics() was called with a wrong number of runs");
    }
    #endif // MYDEBUG
    for(const auto& disconnectInformation : m_disconnectTimes)
    {
        if(disconnectInformation.first)
        {
            m_actinTimeToDisconnectStatistics.addValue(disconnectInformation.second.getTimeToDisconnect());
            m_actinPositionOfDisconnectStatistics.addValue(disconnectInformation.second.getPositionOfDisconnect());
            m_actinTimeLastTrackingCompletionStatistics.addValue(disconnectInformation.second.getTimeLastTrackingCompletion());
            m_actinTotalTimeBehindTipStatistics.addValue(disconnectInformation.second.getTotalTimeBehindTip());
            m_actinTotalTimeOnTipStatistics.addValue(disconnectInformation.second.getTotalTimeOnTip());
        }
    }
}
