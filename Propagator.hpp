#ifndef PROPAGATOR_HPP
#define PROPAGATOR_HPP

#include <cstdint>
#include <string>

#include "SystemState.hpp"
#include "RandomGenerator.hpp"

class Propagator
{
private:
    const int32_t m_nTimeSteps;
    const double m_calcTimeStep;
    const double m_diffusionConstantMicrotubule;
    const double m_springConstant;

public:
    Propagator(const int32_t nTimeSteps, const double calcTimeStep, const double diffusionConstantMicrotubule, const double springConstant);
    ~Propagator();

    // Enter the SystemState as a reference into the run function, such that the propagator can propagate it.
    void run(SystemState& systemState, RandomGenerator& generator);

    void moveMicrotubule();

};

#endif // PROPAGATOR_HPP
