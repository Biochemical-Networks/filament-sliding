#include <string>
#include <iostream>
#include <utility>
#include <limits>
#include <iomanip>

#include "filament-sliding/Histogram.hpp"
#include "filament-sliding/MathematicalFunctions.hpp"
#include "filament-sliding/Statistics.hpp"
#include "filament-sliding/GeneralException.hpp"
#include "filament-sliding/OutputParameters.hpp"

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

void Histogram::addValue(const double value)
{
    Statistics::addValue(value); // First call the functionality of Statistics::addValue(), and then add more functionality

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
        binNumber = MathematicalFunctions::intFloor((value-m_lowestValue)/m_binSize)+1; // +1 for the extremum bin
    }

    ++m_bins.at(binNumber);
}

std::pair<double,double> Histogram::calculateBinBounds(const int32_t binNumber) const
{
    #ifdef MYDEBUG
    if(binNumber<0 || binNumber > m_numberOfBins+1)
    {
        throw GeneralException("Histogram::calculateBinBounds(): binNumber is out of range.");
    }
    #endif // MYDEBUG

    if(binNumber==0)
    {
        return std::pair<double,double>(-std::numeric_limits<double>::infinity(), m_lowestValue);
    }
    else if(binNumber == m_numberOfBins+1)
    {
        return std::pair<double,double>(m_highestValue, std::numeric_limits<double>::infinity());
    }
    else
    {
        const double lowerBoundBin = static_cast<double>(binNumber-1)*m_binSize + m_lowestValue;
        return std::pair<double,double>(lowerBoundBin, lowerBoundBin+m_binSize);
    }
}

std::ostream& operator<< (std::ostream &out, const Histogram &histogram)
{
    for(uint32_t binNumber=0; binNumber < histogram.m_bins.size(); ++binNumber)
    {
        const std::pair<double,double> binBounds = histogram.calculateBinBounds(binNumber);

        out << std::setw(OutputParameters::collumnWidth) << ((binBounds.first==-std::numeric_limits<double>::infinity())?"-infinity":std::to_string(binBounds.first))
            << std::setw(OutputParameters::collumnWidth) << ((binBounds.second==std::numeric_limits<double>::infinity())?"infinity":std::to_string(binBounds.second))
            << std::setw(OutputParameters::collumnWidth) << histogram.m_bins.at(binNumber)
            << std::setw(OutputParameters::collumnWidth) << static_cast<long double>(histogram.m_bins.at(binNumber))/static_cast<long double>(histogram.getNumberOfSamples()) << '\n';
    }
    return out;
}
