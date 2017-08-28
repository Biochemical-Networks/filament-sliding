#include "Reaction.hpp"

Reaction::Reaction(const double elementaryRate)
    :   m_elementaryRate(elementaryRate)
{
}

Reaction::~Reaction()
{
}

void Reaction::resetAccumulatedAction()
{
    m_accumulatedAction = 0.0;
}
