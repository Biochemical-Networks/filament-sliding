#ifndef REMOVESITE_HPP
#define REMOVESITE_HPP

#include "Reaction.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

#include <cstdint>

// Reaction to block a site: label the site to be effectively non-existant.
// Used to make tip region disappear, and the reverse reaction, UnblockSite, is used to make the tip grow

class RemoveSite : public Reaction
{
private:
    const double m_rateRemoveOneBoundSite;
    const double m_rateRemoveOneUnboundSite;

    const bool m_unbindUponBlock;

    int32_t whichSiteToBlock(SystemState& systemState, RandomGenerator& generator);
public:
    RemoveSite(const double rateRemoveOneBoundSite, const double rateRemoveOneUnboundSite, const bool unbindUponBlock);
    ~RemoveSite() override;

    void setCurrentRate(const SystemState& systemState) override;

    void performReaction(SystemState& systemState, RandomGenerator& generator) override;
};

#endif // REMOVESITE_HPP
