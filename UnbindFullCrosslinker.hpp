#ifndef UNBINDFULLCROSSLINKER_HPP
#define UNBINDFULLCROSSLINKER_HPP

#include "Reaction.hpp"
#include "FullConnection.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

#include <vector>

class UnbindFullCrosslinker : public Reaction
{
private:
    const Crosslinker::Type m_typeToUnbind;

    const double m_springConstant;

    std::vector<double> m_individualRates; // store one rate for each member of the CrosslinkerContainer.m_fullConnections

    FullConnection whichToDisconnect(SystemState& systemState, RandomGenerator& generator) const;
public:
    UnbindFullCrosslinker(const double elementaryRate, const Crosslinker::Type typeToUnbind, const double springConstant);
    ~UnbindFullCrosslinker() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;
};

#endif // UNBINDFULLCROSSLINKER_HPP
