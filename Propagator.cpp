#include "Propagator.hpp"
#include <random>
#include <cstdint>
#include <string>

Propagator::Propagator(const std::string seedString, const int32_t nTimeSteps)
    :   m_nTimeSteps(nTimeSteps)
{
    std::seed_seq seed(seedString.begin(), seedString.end());
    m_generator.seed(seed);
}

Propagator::~Propagator()
{
}
