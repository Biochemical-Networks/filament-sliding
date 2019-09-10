#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <cstdint>
#include <iostream>

class Statistics
{
private:
    int64_t m_numberOfSamples; // 64 bit, could hold many steps (nRunBlocks*nStepsPerBlock/probePeriod)

    // use "online algorithm" (https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm)
    double m_mean;
    double m_accumulatedSquaredDeviation;

public:
    Statistics();
    virtual ~Statistics();

    void addValue(const double value);

    int64_t getNumberOfSamples() const;
    double getMean() const;
    double getVariance() const;
    double getSEM() const;

    bool canReportStatistics() const;

    Statistics& operator+=(const Statistics& term);
};

std::ostream& operator<<(std::ostream& out, const Statistics& statistics);

#endif // STATISTICS_HPP
