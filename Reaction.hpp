#ifndef REACTION_HPP
#define REACTION_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"


class Reaction
{
protected:
    const double m_elementaryRate;
    double m_currentRate;

    double m_action; // The summation of the rates at each time step, used for integrating the rate over time
public:
    Reaction(const double elementaryRate);
    virtual ~Reaction(); // Don't allow Reaction pointers to destroy derived objects only using the Reaction destructor

    void resetAction();

    double getAction() const;

    // Pure virtual functions, these have to be present in a reaction, but need to be implemented in the derived classes
    virtual void setCurrentRate(const SystemState& systemState) = 0;

    virtual void updateAction() = 0;

    virtual void performReaction(SystemState& systemState, RandomGenerator& generator) = 0;

};

#endif // REACTION_HPP
