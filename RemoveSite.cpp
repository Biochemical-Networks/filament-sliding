#include "RemoveSite.hpp"

RemoveSite::RemoveSite(const double rateRemoveOneBoundSite, const double rateRemoveOneUnboundSite, const bool unbindUponBlock)
    :   Reaction(),
        m_rateRemoveOneBoundSite(rateRemoveOneBoundSite),
        m_rateRemoveOneUnboundSite(rateRemoveOneUnboundSite),
        m_unbindUponBlock(unbindUponBlock)
{
}

RemoveSite::~RemoveSite()
{
}

void RemoveSite::setCurrentRate(const SystemState& systemState)
{
    m_currentRate = systemState.getNUnblockedSitesFixed(BoundState::BOUND)*m_rateRemoveOneBoundSite
                   +systemState.getNUnblockedSitesFixed(BoundState::UNBOUND)*m_rateRemoveOneUnboundSite;
}

void RemoveSite::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    const int32_t siteToBlock = whichSiteToBlock(systemState, generator);

    // Leave any crosslinkers bound to it after blocking: the new unbinding rate will take care of creating a stationary distribution. (m_unbindUponBlock=false)
    // Only in case that the (un)binding rates to on blocked sites are zero, do unbind them. (m_unbindUponBlock=true)
    systemState.blockSiteOnFixed(siteToBlock, m_unbindUponBlock);
}

std::string RemoveSite::identity() const
{
    return "RemoveSite";
}

int32_t RemoveSite::whichSiteToBlock(SystemState& systemState, RandomGenerator& generator)
{
    // This division must be possible, this function should never be called when its rate is zero
    const double probBoundSiteRemoved = systemState.getNUnblockedSitesFixed(BoundState::BOUND)*m_rateRemoveOneBoundSite / m_currentRate;

    const BoundState boundStateToBlock = generator.getBernoulli(probBoundSiteRemoved) ? BoundState::BOUND : BoundState::UNBOUND;

    const uint32_t numberOfSites = systemState.getNUnblockedSitesFixed(boundStateToBlock);
    const uint32_t label = generator.getUniformInteger(0, numberOfSites-1);
    return systemState.getUnblockedSitePosition(boundStateToBlock, label);
}
