#include "ActinDynamicsEstimate.hpp"
#include "MathematicalFunctions.hpp"
#include "OutputParameters.hpp"

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

double ActinDynamicsEstimate::getDriftVelocity(const uint32_t binNumber) const
{
    return m_movementStatistics.at(binNumber).getMean()/m_estimateTimeStep;
}

double ActinDynamicsEstimate::getDiffusionConstant(const uint32_t binNumber) const
{
    return m_movementStatistics.at(binNumber).getVariance()/(2*m_estimateTimeStep);
}

double ActinDynamicsEstimate::getEffectiveForce(const uint32_t binNumber) const
{
    return getDriftVelocity(binNumber)/getDiffusionConstant(binNumber);
}

std::ostream& operator<<(std::ostream& out, const ActinDynamicsEstimate& dynamicsContainer)
{
    for(int32_t binNumber=0; binNumber < dynamicsContainer.m_numberOfBins; ++binNumber)
    {
        const double lowerBinBound = (binNumber-1)*dynamicsContainer.m_binSize;
        const double upperBinBound = lowerBinBound+dynamicsContainer.m_binSize;

        out << std::setw(OutputParameters::collumnWidth) << ((binNumber==0)?"-infinity":std::to_string(lowerBinBound))
            << std::setw(OutputParameters::collumnWidth) << ((binNumber==dynamicsContainer.m_numberOfBins-1)?"infinity":std::to_string(upperBinBound))
            << std::setw(OutputParameters::collumnWidth) << dynamicsContainer.getDiffusionConstant(binNumber)
            << std::setw(OutputParameters::collumnWidth) << dynamicsContainer.getEffectiveForce(binNumber) << '\n';
    }

}
