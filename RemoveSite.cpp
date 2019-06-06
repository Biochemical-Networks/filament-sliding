#include "RemoveSite.hpp"

RemoveSite::RemoveSite(const double rateRemoveOneSite, const bool unbindUponBlock)
    :   Reaction(),
        m_rateRemoveOneSite(rateRemoveOneSite),
        m_unbindUponBlock(unbindUponBlock)
{
}

RemoveSite::~RemoveSite()
{
}

void RemoveSite::setCurrentRate(const SystemState& systemState)
{
    m_currentRate = systemState.getNUnblockedSitesFixed()*m_rateRemoveOneSite;
}

void RemoveSite::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    const int32_t siteToBlock = whichSiteToBlock(systemState, generator);

    // Leave any crosslinkers bound to it after blocking: the new unbinding rate will take care of creating a stationary distribution. (m_unbindUponBlock=false)
    // Only in case that the (un)binding rates to on blocked sites are zero, do unbind them. (m_unbindUponBlock=true)
    systemState.blockSiteOnFixed(siteToBlock, m_unbindUponBlock);
}

int32_t RemoveSite::whichSiteToBlock(SystemState& systemState, RandomGenerator& generator)
{
    const uint32_t numberOfSites = systemState.getNUnblockedSitesFixed();
    const uint32_t label = generator.getUniformInteger(0, numberOfSites-1);
    return systemState.getUnblockedSitePosition(label);
}
