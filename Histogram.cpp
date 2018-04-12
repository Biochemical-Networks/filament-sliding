#include "Histogram.hpp"
#include "MathematicalFunctions.hpp"

Histogram::Histogram(const double binSize, const double lowestValue, const double highestValue)
    :   m_binSize(binSize),
        m_lowestValue(lowestValue),
        m_highestValue(highestValue),
        m_numberOfBins(MathematicalFunctions::alternativeIntCeil((highestValue-lowestValue)/binSize)+2), // alternativeIntCeil for bins are left closed-right open, +2 for binning out of range
        m_bins(m_numberOfBins,0) // initially fill the bins with zero counts
{
}

Histogram::~Histogram()
{
}
