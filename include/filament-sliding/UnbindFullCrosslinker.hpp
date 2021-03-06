#ifndef UNBINDFULLCROSSLINKER_HPP
#define UNBINDFULLCROSSLINKER_HPP

#include <vector>

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/FullConnection.hpp"
#include "filament-sliding/RandomGenerator.hpp"
#include "filament-sliding/Reaction.hpp"
#include "filament-sliding/SystemState.hpp"

/* The Reaction that unbinds a fully connected linker such that it becomes
 * partially connected. This class needs to decide which full linker
 * disconnects, and which terminus, and only reports the total unbinding rate of
 * all full linkers of this type to the public.
 */

class UnbindFullCrosslinker: public Reaction {
  private:
    const double m_rateOneLinkerUnbinds;
    const Crosslinker::Type m_typeToUnbind;
    const double m_probHeadUnbinds;

    const double m_springConstant;

    std::vector<double>
            m_individualRates; // store one rate for each member of the
                               // CrosslinkerContainer.m_fullConnections

    FullConnection whichToDisconnect(
            SystemState& systemState,
            RandomGenerator& generator) const;

  public:
    UnbindFullCrosslinker(
            const double rateOneLinkerUnbinds,
            const Crosslinker::Type typeToUnbind,
            const double headBiasEnergy,
            const double springConstant);
    ~UnbindFullCrosslinker() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator)
            override;
};

#endif // UNBINDFULLCROSSLINKER_HPP
