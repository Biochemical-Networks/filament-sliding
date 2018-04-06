#include "Statistics.hpp"
#include "GeneralException/GeneralException.hpp"

#include <cstdint>

Statistics::Statistics()
    :   m_numberOfSamples(0),
        m_empty(true),
        m_accumulatedDifference(0),
        m_accumulatedSquaredDifference(0)
{
}

Statistics::~Statistics()
{
}

void Statistics::addValue(const double value)
{
    ++m_numberOfSamples;
    if(m_empty)
    {
        m_firstValue = value;
        m_empty = false;
    }
    const double difference = value-m_firstValue;
    m_accumulatedDifference += difference;
    m_accumulatedSquaredDifference += difference*difference;
}

double Statistics::getMean() const
{
    if(m_empty)
    {
        throw GeneralException("Statistics::getMean() was called when there were no samples");
    }
    return m_firstValue + m_accumulatedDifference/static_cast<double>(m_numberOfSamples);
}

double Statistics::getVariance() const
{
    if(m_empty||m_numberOfSamples<=1)
    {
        throw GeneralException("Statistics::getVariance was called with insufficient samples");
    }
    const double n = static_cast<double>(m_numberOfSamples);
    return (m_accumulatedSquaredDifference-m_accumulatedDifference*m_accumulatedDifference/n)/(n-1);
}


bool Statistics::isEmpty() const
{
    return m_empty;
}
