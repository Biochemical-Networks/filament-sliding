#include "Histogram.hpp"
#include "MathematicalFunctions.hpp"
#include "Statistics.hpp"
#include "GeneralException/GeneralException.hpp"

Histogram::Histogram(const double binSize, const double lowestValue, const double highestValue)
    :   m_binSize(binSize),
        m_lowestValue(lowestValue),
        m_numberOfBins(MathematicalFunctions::alternativeIntCeil((highestValue-lowestValue)/binSize)), // alternativeIntCeil for bins are left closed-right open
        m_highestValue(lowestValue + m_numberOfBins*m_binSize), // The true highest value, which sets the exclusive upper bound which is counted within the non-extremal bins
        m_bins(m_numberOfBins+2,0) // +2 for binning out of range (extremal bins). Initially fill the bins with zero counts
{
    #ifdef MYDEBUG
    if(binSize<=0 || lowestValue>=highestValue)
    {
        throw GeneralException("Histogram::Histogram() encountered irregular input parameters.");
    }
    #endif // MYDEBUG
}

Histogram::~Histogram()
{
}

void Histogram::addDataPoint(const double value)
{
    int32_t binNumber;

    if(value < m_lowestValue)
    {
        binNumber = 0; // lower extremum bin
    }
    else if(value >= m_highestValue)
    {
        binNumber = m_numberOfBins+1; // upper extremum bin
    }
    else
    {
        binNumber = MathematicalFunctions::intFloor(value-m_lowestValue/m_binSize)+1; // +1 for the extremum bin
    }

    ++m_bins.at(binNumber);

    m_distributionStatistics.addValue(value);
}

int64_t Histogram::getNumberOfSamples() const
{
    return m_distributionStatistics.getNumberOfSamples();
}
double Histogram::getMean() const
{
    return m_distributionStatistics.getMean();
}

double Histogram::getVariance() const
{
    return m_distributionStatistics.getVariance();
}

double Histogram::getSEM() const
{
    return m_distributionStatistics.getSEM();
}
