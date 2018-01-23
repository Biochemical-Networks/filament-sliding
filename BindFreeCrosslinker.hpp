#ifndef BINDFREECROSSLINKER_HPP
#define BINDFREECROSSLINKER_HPP

#include "Reaction.hpp"
#include "SystemState.hpp"
#include "Crosslinker.hpp"
#include "RandomGenerator.hpp"

#include <cstdint>

class BindFreeCrosslinker : public Reaction
{
private:
    const Crosslinker::Type m_typeToBind;

    void whereToConnect(const SystemState& systemState, RandomGenerator& generator, Extremity::MicrotubuleType& microtubuleToConnect, int32_t& siteToConnect);
public:
    BindFreeCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind);
    virtual ~BindFreeCrosslinker();

    virtual void setCurrentRate(const SystemState& systemState) override;

    //virtual void updateAction() override;

    virtual void performReaction(SystemState& systemState, RandomGenerator& generator) override;
};

#endif // BINDFREECROSSLINKER_HPP
