#include "Initialiser.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

Initialiser::Initialiser(const double initialPositionMicrotubule, const double fractionConnectedCrosslinkers, const InitialCrosslinkerDistribution method)
    :   m_initialPositionMicrotubule(initialPositionMicrotubule),
        m_fractionConnectedCrosslinkers(fractionConnectedCrosslinkers),
        m_method(method)
{
}

Initialiser::~Initialiser()
{
}

void Initialiser::initialise(SystemState& systemState, RandomGenerator& generator)
{
    systemState.setMicrotubulePosition(m_initialPositionMicrotubule);
}
