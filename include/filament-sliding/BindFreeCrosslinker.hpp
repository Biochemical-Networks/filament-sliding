#ifndef BINDFREECROSSLINKER_HPP
#define BINDFREECROSSLINKER_HPP

#include <cstdint>

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/MicrotubuleType.hpp"
#include "filament-sliding/RandomGenerator.hpp"
#include "filament-sliding/Reaction.hpp"
#include "filament-sliding/SystemState.hpp"

/* The Reaction that binds a free linker such that it becomes partially
 * connected. This class needs to decide where the free linker connects, and
 * only reports the total binding rate to the public.
 */

class BindFreeCrosslinker: public Reaction {
  private:
    const double m_rateToOneSite;
    const Crosslinker::Type m_typeToBind;
    const double m_probHeadBinds;

    SiteLocation whereToConnect(
            const SystemState& systemState,
            RandomGenerator& generator) const;

  public:
    BindFreeCrosslinker(
            const double rateToOneSite,
            const Crosslinker::Type typeToBind,
            const double headBiasEnergy);
    ~BindFreeCrosslinker() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator)
            override;
};

#endif // BINDFREECROSSLINKER_HPP
