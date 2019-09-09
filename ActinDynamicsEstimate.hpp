#ifndef ACTINDYNAMICSESTIMATE_HPP
#define ACTINDYNAMICSESTIMATE_HPP

#include <vector>
#include <cstdint>
#include <iostream>
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

    double getDriftVelocity(const uint32_t binNumber) const;

    double getDiffusionConstant(const uint32_t binNumber) const;

    double getEffectiveForce(const uint32_t binNumber) const;

    friend std::ostream& operator<<(std::ostream& out, const ActinDynamicsEstimate& dynamicsContainer);
};

std::ostream& operator<<(std::ostream& out, const ActinDynamicsEstimate& dynamicsContainer);

#endif // ACTINDYNAMICSESTIMATE_HPP