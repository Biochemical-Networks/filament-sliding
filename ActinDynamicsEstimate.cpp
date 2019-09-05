#include "ActinDynamicsEstimate.hpp"
#include "MathematicalFunctions.hpp"

ActinDynamicsEstimate::ActinDynamicsEstimate(const double binSize,
                      const double estimateTimeStep,
                      const double tipSize)
    :   m_binSize(binSize),
        m_estimateTimeStep(estimateTimeStep),
        m_numberOfBins(MathematicalFunctions::intCeil(tipSize/binSize)+2), // have at least enough bins to cover the whole tip, 1 for everything behind the tip, and 1 for further away
        m_movementStatistics(m_numberOfBins)
{
}

ActinDynamicsEstimate::~ActinDynamicsEstimate()
{
}

void ActinDynamicsEstimate::addPositionRelativeToTipBegin(const double value)
{
    if(value<0.0) // actin fully on lattice
    {
        m_movementStatistics.front().addValue(value);
    }
    else if(value >= (m_numberOfBins-2)*m_binSize) // actin in front of tip
    {
        m_movementStatistics.back().addValue(value);
    }
    else // 0.0 <= value < end of specific bins
    {
        const int32_t binNumber = 1 + MathematicalFunctions::intFloor(value/m_binSize); // binNumber 1 is the first possibility, binNumber m_numberOfBins-2 the last
        m_movementStatistics.at(binNumber).addValue(value);
    }
}

