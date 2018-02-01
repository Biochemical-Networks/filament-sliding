#ifndef BINDFREECROSSLINKER_HPP
#define BINDFREECROSSLINKER_HPP

#include "Reaction.hpp"
#include "SystemState.hpp"
#include "Crosslinker.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleType.hpp"

#include <cstdint>

class BindFreeCrosslinker : public Reaction
{
private:
    const Crosslinker::Type m_typeToBind;

    SiteLocation whereToConnect(const SystemState& systemState, RandomGenerator& generator);
public:
    BindFreeCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind);
    virtual ~BindFreeCrosslinker();

    virtual void setCurrentRate(const SystemState& systemState) override;

    virtual void performReaction(SystemState& systemState, RandomGenerator& generator) override;
};

#endif // BINDFREECROSSLINKER_HPP
