#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <cstdint>

class Statistics {
  private:
    int64_t m_numberOfSamples; // 64 bit, could hold many steps
                               // (nRunBlocks*nStepsPerBlock/probePeriod)

    // use "online algorithm"
    // (https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm)
    double m_value;
    double m_mean;
    double m_previousMean;
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
};

#endif // STATISTICS_HPP
