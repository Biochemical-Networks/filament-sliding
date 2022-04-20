#include "filament-sliding/Reaction.hpp"

Reaction::Reaction() {
    resetAction(); // set initial action to zero
}

Reaction::~Reaction() {}

void Reaction::resetAction() { m_action = 0.0; }

double Reaction::getAction() const { return m_action; }

void Reaction::updateAction() {
    // We let the action have the same units as the rate (s^-1), since this
    // doesn't require constant multiplication with the time step. Hence, when
    // calculating a threshold for the action, we do need to take the time step
    // into account
    m_action += m_currentRate;
}

double Reaction::getCurrentRate() const { return m_currentRate; }
