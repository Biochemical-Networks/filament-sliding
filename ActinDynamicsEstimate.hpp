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

    ActinDynamicsEstimate(const ActinDynamicsEstimate&) = delete;
    ActinDynamicsEstimate& operator=(const ActinDynamicsEstimate&) = delete;
    ActinDynamicsEstimate(ActinDynamicsEstimate&&) = default;
    ActinDynamicsEstimate& operator=(ActinDynamicsEstimate&&) = default;

    void addPositionRelativeToTipBegin(const double initialPosition, const double positionChange);

    ActinDynamicsEstimate& operator+=(const ActinDynamicsEstimate& term);

    double getDriftVelocity(const uint32_t binNumber) const;

    double getDiffusionConstant(const uint32_t binNumber) const;

    double getEffectiveForce(const uint32_t binNumber) const;

    double getEstimateTimeStep() const;

    int64_t getNSamples() const;

    friend std::ostream& operator<<(std::ostream& out, const ActinDynamicsEstimate& dynamicsContainer);
};

std::ostream& operator<<(std::ostream& out, const ActinDynamicsEstimate& dynamicsContainer);

#endif // ACTINDYNAMICSESTIMATE_HPP
