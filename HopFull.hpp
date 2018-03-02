#ifndef HOPFULL_HPP
#define HOPFULL_HPP

#include "Reaction.hpp"


class HopFull : public Reaction
{
private:
    const Crosslinker::Type m_typeToHop;

    const double m_springConstant; // k/(k_B T)

    // For vanishing spring constant, the rates of hopping towards the plus or minus tip obey r+/r- = exp(hopToPlusBiasEnergy),
    // with the choice r+ = elementaryRate*exp(hopToPlusBias/2) and r- = elementaryRate*exp(-hopToPlusBias/2)
    const double m_headHopToPlusBaseRate;
    const double m_headHopToMinusBaseRate;
    const double m_tailHopToPlusBaseRate;
    const double m_tailHopToMinusBaseRate;

    std::vector<double> m_individualRates; // store one rate for each member of the CrosslinkerContainer.m_possiblePartialHops

    double getBaseRateToHop(const Crosslinker::Terminus terminusToHop, const HopDirection directionToHop) const;

    PossibleFullHop whichHop(const SystemState& systemState, RandomGenerator& generator) const;

public:
    HopFull(const double elementaryRate, const Crosslinker::Type typeToHop, const double springConstant,
                const double headHopToPlusBiasEnergy, const double tailHopToPlusBiasEnergy);
    ~HopFull() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;
};

#endif // HOPFULL_HPP
