#ifndef REACTION_HPP
#define REACTION_HPP

#include "SystemState.hpp"


class Reaction
{
private:
    const double m_elementaryRate;
    double m_currentRate;

    double m_accumulatedAction; // The summation of the rates at each time step, used for integrating the rate over time
public:
    Reaction(const double elementaryRate);
    ~Reaction();

    void resetAccumulatedAction();

};

#endif // REACTION_HPP
