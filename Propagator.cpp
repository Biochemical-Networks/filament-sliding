#include "Propagator.hpp"

#include "RandomGenerator.hpp"
#include "SystemState.hpp"
#include "Output.hpp"

#include <cstdint>
#include <string>
#include <random>
#include <cmath>

#include <iostream>

Propagator::Propagator(const int32_t nTimeSteps, const double calcTimeStep, const double diffusionConstantMicrotubule, const double springConstant)
    :   m_nTimeSteps(nTimeSteps),
        m_calcTimeStep(calcTimeStep),
        m_diffusionConstantMicrotubule(diffusionConstantMicrotubule),
        m_springConstant(springConstant),
        m_deviationMicrotubule(std::sqrt(2*m_diffusionConstantMicrotubule*m_calcTimeStep))
{
}

Propagator::~Propagator()
{
}

void Propagator::run(SystemState& systemState, RandomGenerator& generator, Output& output)
{
    for (int32_t timeStep = 0; timeStep < m_nTimeSteps; ++timeStep)
    {
        if (timeStep%10==0)
            output.writeMicrotubulePosition(timeStep*m_calcTimeStep, systemState);

        moveMicrotubule(systemState, generator);
    }
    output.writeMicrotubulePosition(m_nTimeSteps*m_calcTimeStep, systemState);

}

void Propagator::moveMicrotubule(SystemState& systemState, RandomGenerator& generator)
{
    double change = generator.getGaussian(0.0, m_deviationMicrotubule);
    systemState.update(change);
}
