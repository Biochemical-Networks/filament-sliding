#ifndef REACTION_HPP
#define REACTION_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"


// Reactions set the rules by which the SystemState is changed. To change it, it uses methods of SystemState, and Reactions are called by the Propagator

class Reaction
{
protected:
    const double m_elementaryRate;
    double m_currentRate;

    double m_action; // The summation of the rates at each time step, used for integrating the rate over time (time steps are assumed fixed)
public:
    Reaction(const double elementaryRate);
    virtual ~Reaction(); // Don't allow Reaction pointers to destroy derived objects only using the Reaction destructor

    void resetAction();

    double getAction() const; // does not have time step duration included

    void updateAction(); // Action is updated by adding the current rate times the time step size to it. Once the total action of all reactions reaches a (randomly set) plateau value, one fires

    // Pure virtual functions, these have to be present in a reaction, but need to be implemented in the derived classes
    virtual void setCurrentRate(const SystemState& systemState) = 0;

    virtual void performReaction(SystemState& systemState, RandomGenerator& generator) = 0;

};

#endif // REACTION_HPP
