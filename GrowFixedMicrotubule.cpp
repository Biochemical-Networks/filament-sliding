#include "GrowFixedMicrotubule.hpp"

GrowFixedMicrotubule::GrowFixedMicrotubule(const double growthRate)
    :   Reaction(),
        m_growthRate(growthRate)
{
    m_currentRate = m_growthRate; // always constant
}

GrowFixedMicrotubule::~GrowFixedMicrotubule()
{
}

void GrowFixedMicrotubule::setCurrentRate(const SystemState& systemState)
{
    // Do nothing, m_currentRate is set once in the constructor instead.
    (void)systemState; // SystemState not used, since rate is independent of it
}

void GrowFixedMicrotubule::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    systemState.growFixed();
    (void)generator; // Do not use generator, since reaction is deterministic once it is needed. Only the time at which it occurs is random.
}

std::string GrowFixedMicrotubule::identity() const
{
    return "GrowFixedMicrotubule";
}
