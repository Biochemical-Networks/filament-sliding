#include "RemoveSite.hpp"

RemoveSite::RemoveSite(const double rateRemoveOneSite)
    :   Reaction(),
        m_rateRemoveOneSite(rateRemoveOneSite)
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
    systemState.blockSiteOnFixed(siteToBlock);
}

int32_t RemoveSite::whichSiteToBlock(SystemState& systemState, RandomGenerator& generator)
{
    const uint32_t numberOfSites = systemState.getNUnblockedSitesFixed();
    const uint32_t label = generator.getUniformInteger(0, numberOfSites-1);
    return systemState.getUnblockedSitePosition(label);
}
