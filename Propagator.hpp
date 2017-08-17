#ifndef PROPAGATOR_HPP
#define PROPAGATOR_HPP

#include <cstdint>
#include <string>

#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"

class Propagator
{
private:
    const int32_t m_nTimeSteps;
    const double m_calcTimeStep;
    const int32_t m_probePeriod; // Holds the number of time steps that are between two probes of the system, meaning that data will be written every so many time steps
    const double m_diffusionConstantMicrotubule;
    const double m_springConstant;

    const double m_deviationMicrotubule;


    void moveMicrotubule(SystemState& systemState, RandomGenerator& generator);

public:
    Propagator(const int32_t nTimeSteps, const double calcTimeStep, const int32_t probePeriod, const double diffusionConstantMicrotubule, const double springConstant);
    ~Propagator();

    // Enter the SystemState as a reference into the run function, such that the propagator can propagate it.
    void run(SystemState& systemState, RandomGenerator& generator, Output& output);

};

#endif // PROPAGATOR_HPP
