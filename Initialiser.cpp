#include "Initialiser.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

Initialiser::Initialiser(const double initialPositionMicrotubule)
    :   m_initialPositionMicrotubule(initialPositionMicrotubule)
{
}

Initialiser::~Initialiser()
{
}

void Initialiser::initialise(SystemState& systemState, RandomGenerator& generator)
{
    systemState.initiate(m_initialPositionMicrotubule);
}
