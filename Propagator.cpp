#include "Propagator.hpp"

#include <cstdint>
#include <string>

Propagator::Propagator(const int32_t nTimeSteps, const double calcTimeStep, const double diffusionConstantMicrotubule, const double springConstant)
    :   m_nTimeSteps(nTimeSteps),
        m_calcTimeStep(calcTimeStep),
        m_diffusionConstantMicrotubule(diffusionConstantMicrotubule),
        m_springConstant(springConstant)
{
}

Propagator::~Propagator()
{
}

