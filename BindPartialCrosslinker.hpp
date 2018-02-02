#ifndef BINDPARTIALCROSSLINKER_HPP
#define BINDPARTIALCROSSLINKER_HPP

#include "Reaction.hpp"
#include "SystemState.hpp"
#include "Crosslinker.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleType.hpp"

#include <cstdint>
#include <vector>

class BindPartialCrosslinker : public Reaction
{
public:
    struct possibleFullConnection
    {
        Crosslinker* p_partialLinker;
        SiteLocation location;
        double extension;
    };

private:
    const Crosslinker::Type m_typeToBind;

    SiteLocation whereToConnect(const SystemState& systemState, RandomGenerator& generator);

    // Stores all possible connections such that the search needs to be done once every time step
    std::vector<possibleFullConnection> possibleConnections; // To be reset every time step

public:
    BindPartialCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind);
    virtual ~BindPartialCrosslinker();

    virtual void setCurrentRate(const SystemState& systemState) override;

    virtual void performReaction(SystemState& systemState, RandomGenerator& generator) override;

    void setPossibleConnections(SystemState& systemState);
};

#endif // BINDPARTIALCROSSLINKER_HPP

