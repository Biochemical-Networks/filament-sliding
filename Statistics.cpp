#include "Statistics.hpp"
#include "GeneralException/GeneralException.hpp"

#include <cstdint>
#include <cmath>

Statistics::Statistics()
    :   m_numberOfSamples(0),
        m_mean(0.0),
        m_previousMean(0.0), // not necessary
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
    m_previousMean = m_mean;
    m_mean += (value - m_previousMean)/static_cast<double>(m_numberOfSamples);

    // Calculate the new (n-1)*variance estimate:
    m_accumulatedSquaredDeviation += (value-m_mean)*(value-m_previousMean);
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
