#ifndef HOPPARTIAL_HPP
#define HOPPARTIAL_HPP

#include "Reaction.hpp"
#include "Crosslinker.hpp"

/* The Reaction that hops the connected terminus of a partially connected linker to either side.
 * The head and tail can have a bias to hop in a specific direction on the polarised microtubule.
 * This class needs to decide which of all possible hops is performed, and only reports the total hopping rate of all partial linkers of this type to the public.
 */

class HopPartial : public Reaction
{
private:
    const Crosslinker::Type m_typeToHop;

    // The rates of hopping towards the plus or minus tip obey r+/r- = exp(hopToPlusBiasEnergy),
    // with the choice r+ = elementaryRate*exp(hopToPlusBias/2) and r- = elementaryRate*exp(-hopToPlusBias/2)
    const double m_headHopToPlusRate;
    const double m_headHopToMinusRate;
    const double m_tailHopToPlusRate;
    const double m_tailHopToMinusRate;

    std::vector<double> m_individualRates; // store one rate for each member of the CrosslinkerContainer.m_possiblePartialHops

    double getRateToHop(const Crosslinker::Terminus terminusToHop, const HopDirection directionToHop) const;

    PossiblePartialHop whichHop(const SystemState& systemState, RandomGenerator& generator) const;

public:
    HopPartial(const double baseRateHead,
               const double baseRateTail,
               const Crosslinker::Type typeToHop,
               const double headHopToPlusBiasEnergy,
               const double tailHopToPlusBiasEnergy);
    ~HopPartial() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;
};

#endif // HOPPARTIAL_HPP
