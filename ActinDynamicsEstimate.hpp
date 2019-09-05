#ifndef ACTINDYNAMICSESTIMATE_HPP
#define ACTINDYNAMICSESTIMATE_HPP

#include <vector>
#include <cstdint>
#include "Statistics.hpp"

class ActinDynamicsEstimate
{
private:
    const double m_binSize;
    const double m_estimateTimeStep;

    const int32_t m_numberOfBins;

    std::vector<Statistics> m_movementStatistics;

public:
    ActinDynamicsEstimate(const double binSize,
                          const double estimateTimeStep,
                          const double tipSize);
    ~ActinDynamicsEstimate();

    void addPositionRelativeToTipBegin(const double value);

    ActinDynamicsEstimate& operator+=(const ActinDynamicsEstimate& term);
};

#endif // ACTINDYNAMICSESTIMATE_HPP
