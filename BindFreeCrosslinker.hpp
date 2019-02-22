#ifndef BINDFREECROSSLINKER_HPP
#define BINDFREECROSSLINKER_HPP

#include "Reaction.hpp"
#include "SystemState.hpp"
#include "Crosslinker.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleType.hpp"

#include <cstdint>

/* The Reaction that binds a free linker such that it becomes partially connected.
 * This class needs to decide where the free linker connects, and only reports the total binding rate to the public.
 */

class BindFreeCrosslinker : public Reaction
{
private:
    const double m_rateOneLinkerToOneSite;
    const Crosslinker::Type m_typeToBind;
    /*const double m_probHeadBinds;*/

    SiteLocation whereToConnect(const SystemState& systemState, RandomGenerator& generator) const;
public:
    BindFreeCrosslinker(const double rateOneLinkerToOneSite, const Crosslinker::Type typeToBind);
    ~BindFreeCrosslinker() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;
};

#endif // BINDFREECROSSLINKER_HPP
