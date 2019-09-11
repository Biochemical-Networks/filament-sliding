#include "Statistics.hpp"
#include "GeneralException/GeneralException.hpp"

#include <cstdint>
#include <cmath>
#include <iostream>
#include <iomanip>
#include "OutputParameters.hpp"

// use "online algorithm" (https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm) for calculating mean and variance.

Statistics::Statistics()
    :   m_numberOfSamples(0),
        m_mean(0.0),
        m_accumulatedSquaredDeviation(0.0)
{
}

Statistics::~Statistics()
{
}

void Statistics::addValue(const double value)
{
    ++m_numberOfSamples;
    // Calculate the new mean estimate:
    double previousMean = m_mean;
    m_mean += (value - previousMean)/static_cast<double>(m_numberOfSamples);

    // Calculate the new (n-1)*variance estimate:
    m_accumulatedSquaredDeviation += (value-m_mean)*(value-previousMean);
}

int64_t Statistics::getNumberOfSamples() const
{
    return m_numberOfSamples;
}

double Statistics::getMean() const
{
    if(m_numberOfSamples==0)
    {
        throw GeneralException("Statistics::getMean() was called when there were no samples");
    }
    return m_mean;
}

double Statistics::getVariance() const
{
    if(m_numberOfSamples < 2)
    {
        throw GeneralException("Statistics::getVariance was called with insufficient samples");
    }
    return m_accumulatedSquaredDeviation/static_cast<double>(m_numberOfSamples-1);
}

double Statistics::getSEM() const
{
    double variance = getVariance();
    return std::sqrt(variance/static_cast<double>(m_numberOfSamples));
}

bool Statistics::canReportStatistics() const
{
    return (m_numberOfSamples>1); // For the variance, at least 2 values are necessary
}

Statistics& Statistics::operator+=(const Statistics& term)
{
    // Use Chan et al. algorithm, "parallel algorithm" (https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Parallel_algorithm)
    const int64_t oldNumberOfSamples=m_numberOfSamples;
    const double meanDifference=term.m_mean - m_mean;
    m_numberOfSamples+=term.m_numberOfSamples;
    if(m_numberOfSamples!=0) // Otherwise you would get 0/0. Both the mean and accumulatedSquaredDeviation remain 0 when neither has samples
    {
        m_mean = (m_mean*oldNumberOfSamples+term.m_mean*term.m_numberOfSamples)/static_cast<double>(m_numberOfSamples);
        m_accumulatedSquaredDeviation += term.m_accumulatedSquaredDeviation
            + static_cast<double>(oldNumberOfSamples)*term.m_numberOfSamples/static_cast<double>(m_numberOfSamples)*meanDifference*meanDifference;
    }
    return (*this);
}

std::ostream& operator<<(std::ostream& out, const Statistics& statistics)
{
    out << std::setw(OutputParameters::collumnWidth) << statistics.getNumberOfSamples()
        << std::setw(OutputParameters::collumnWidth) << statistics.getMean()
        << std::setw(OutputParameters::collumnWidth) << statistics.getVariance()
        << std::setw(OutputParameters::collumnWidth) << statistics.getSEM() << '\n';
    return out;
}
