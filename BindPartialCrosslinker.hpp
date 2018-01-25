#ifndef BINDPARTIALCROSSLINKER_HPP
#define BINDPARTIALCROSSLINKER_HPP

#include "Reaction.hpp"
#include "SystemState.hpp"
#include "Crosslinker.hpp"
#include "RandomGenerator.hpp"

#include <cstdint>

class BindPartialCrosslinker : public Reaction
{
private:
    const Crosslinker::Type m_typeToBind;

    // The following function returns through references
    void whereToConnect(const SystemState& systemState, RandomGenerator& generator, Extremity::MicrotubuleType& microtubuleToConnect, int32_t& siteToConnect);
public:
    BindPartialCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind);
    virtual ~BindPartialCrosslinker();

    virtual void setCurrentRate(const SystemState& systemState) override;

    virtual void performReaction(SystemState& systemState, RandomGenerator& generator) override;
};

#endif // BINDPARTIALCROSSLINKER_HPP

