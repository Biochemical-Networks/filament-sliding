#ifndef UNBINDPARTIALCROSSLINKER_HPP
#define UNBINDPARTIALCROSSLINKER_HPP

#include "Reaction.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

/* The Reaction that unbinds a partially connected linker such that it becomes free.
 * This class needs to decide which partial linker disconnects, and only reports the total unbinding rate to the public.
 */

class UnbindPartialCrosslinker : public Reaction
{
private:
    const double m_rateOneTerminusDisconnects;
    const Crosslinker::Type m_typeToUnbind;
    // The following gives the factor with which the rate needs to be multiplied when it involves unbinding the head (tail).
    const double m_headUnbindingFactor; // Equals 2/(1+exp(headBindingBiasEnergy)) = 2*prob_tail_binds. m_tailUnbindingFactor = 2-m_headUnbindingFactor
    const double m_tailUnbindingFactor;

    Crosslinker& whichToDisconnect(SystemState& systemState, RandomGenerator& generator) const;
public:
    UnbindPartialCrosslinker(const double rateOneTerminusDisconnects, const Crosslinker::Type typeToUnbind, const double headBiasEnergy);
    ~UnbindPartialCrosslinker() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;

};

#endif // UNBINDPARTIALCROSSLINKER_HPP
