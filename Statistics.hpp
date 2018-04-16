#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <cstdint>

class Statistics
{
private:
    int64_t m_numberOfSamples; // 64 bit, could hold many steps (nRunBlocks*nStepsPerBlock/probePeriod)
    double m_firstValue; // Always subtract the first value to prevent catastrophic cancellation
    bool m_empty;
    double m_accumulatedDifference;
    double m_accumulatedSquaredDifference;
public:
    Statistics();
    virtual ~Statistics();

    void addValue(const double value);

    int64_t getNumberOfSamples() const;
    double getMean() const;
    double getVariance() const;
    double getSEM() const;

    bool canReportStatistics() const;
};

#endif // STATISTICS_HPP
