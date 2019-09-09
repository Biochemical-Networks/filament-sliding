#include "ActinDynamicsEstimate.hpp"
#include "MathematicalFunctions.hpp"
#include "OutputParameters.hpp"

#include <iomanip>

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

void ActinDynamicsEstimate::addPositionRelativeToTipBegin(const double initialPosition, const double positionChange)
{
    if(initialPosition<0.0) // actin fully on lattice
    {
        m_movementStatistics.front().addValue(positionChange);
    }
    else if(initialPosition >= (m_numberOfBins-2)*m_binSize) // actin in front of tip
    {
        m_movementStatistics.back().addValue(positionChange);
    }
    else // 0.0 <= initialPosition < end of specific bins
    {
        const int32_t binNumber = 1 + MathematicalFunctions::intFloor(initialPosition/m_binSize); // binNumber 1 is the first possibility, binNumber m_numberOfBins-2 the last
        m_movementStatistics.at(binNumber).addValue(positionChange);
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

double ActinDynamicsEstimate::getEstimateTimeStep() const
{
    return m_estimateTimeStep;
}

int64_t ActinDynamicsEstimate::getNSamples() const
{
    int64_t answer=0;
    for(const Statistics& statistics : m_movementStatistics)
    {
        answer += statistics.getNumberOfSamples();
    }
    return answer;
}

std::ostream& operator<<(std::ostream& out, const ActinDynamicsEstimate& dynamicsContainer)
{
    for(int32_t binNumber=0; binNumber < dynamicsContainer.m_numberOfBins; ++binNumber)
    {
        const double lowerBinBound = (binNumber-1)*dynamicsContainer.m_binSize;
        const double upperBinBound = lowerBinBound+dynamicsContainer.m_binSize;

        const int64_t numberOfSamples = dynamicsContainer.m_movementStatistics.at(binNumber).getNumberOfSamples();

        out << std::setw(OutputParameters::collumnWidth) << ((binNumber==0)?"-infinity":std::to_string(lowerBinBound))
            << std::setw(OutputParameters::collumnWidth) << ((binNumber==dynamicsContainer.m_numberOfBins-1)?"infinity":std::to_string(upperBinBound))
            << std::setw(OutputParameters::collumnWidth) << numberOfSamples
            << std::setw(OutputParameters::collumnWidth) << ((numberOfSamples<2)?0:dynamicsContainer.getDiffusionConstant(binNumber))
            << std::setw(OutputParameters::collumnWidth) << ((numberOfSamples==0)?0:dynamicsContainer.getEffectiveForce(binNumber)) << '\n';
    }
    return out;
}
