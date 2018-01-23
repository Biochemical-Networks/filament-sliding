#include "Reaction.hpp"

Reaction::Reaction(const double elementaryRate)
    :   m_elementaryRate(elementaryRate)
{
    resetAction(); // set initial action to zero
}

Reaction::~Reaction()
{
}

void Reaction::resetAction()
{
    m_action = 0.0;
}


double Reaction::getAction() const
{
    return m_action;
}

void Reaction::updateAction()
{
    m_action += m_currentRate;
}
