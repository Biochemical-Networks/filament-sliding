#include "ConnectedTipGrowth.hpp"

ConnectedTipGrowth::ConnectedTipGrowth(const double growthRate, const bool unbindUponBlock)
    :   Reaction(),
        m_growthRate(growthRate),
        m_unbindUponBlock(unbindUponBlock)
{
    m_currentRate=growthRate; // Always constant
}

ConnectedTipGrowth::~ConnectedTipGrowth()
{
}

void ConnectedTipGrowth::setCurrentRate(const SystemState& systemState)
{
    // Do nothing, m_currentRate is set once in the constructor instead.
    (void)systemState; // SystemState not used, since rate is independent of it
}

void ConnectedTipGrowth::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    (void)generator; // Do not use generator, since reaction is deterministic once it is needed. Only the time at which it occurs is random.

    systemState.growFixed();

    const int32_t siteToBlock = systemState.getNSites(MicrotubuleType::FIXED)-systemState.getNUnblockedSitesFixed()-1;

    // Leave any crosslinkers bound to it after blocking: the new unbinding rate will take care of creating a stationary distribution. (m_unbindUponBlock=false)
    // Only in case that the (un)binding rates to on blocked sites are zero, do unbind them. (m_unbindUponBlock=true)

    // assume that the tip size is always fixed from the beginning: the initial microtubule must be at least as long as the tip
    systemState.blockSiteOnFixed(siteToBlock, m_unbindUponBlock);
}

int32_t RemoveSite::whichSiteToBlock(SystemState& systemState, RandomGenerator& generator)
{
    // This division must be possible, this function should never be called when its rate is zero
    const double probBoundSiteRemoved = systemState.getNUnblockedSitesFixed(BoundState::BOUND)}
