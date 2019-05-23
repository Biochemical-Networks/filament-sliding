#include "BindFreeCrosslinker.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleType.hpp"

#include <cstdint>
#include <cmath>

BindFreeCrosslinker::BindFreeCrosslinker(const double rateOneLinkerToOneSiteTip, const double rateOneLinkerToOneSiteBlocked, const Crosslinker::Type typeToBind)
    :   Reaction(),
        m_rateOneLinkerToOneSiteTip(rateOneLinkerToOneSiteTip),
        m_rateOneLinkerToOneSiteBlocked(rateOneLinkerToOneSiteBlocked),
        m_typeToBind(typeToBind)
{
}

BindFreeCrosslinker::~BindFreeCrosslinker()
{
}

void BindFreeCrosslinker::setCurrentRate(const SystemState& systemState)
{
    // make rate independent from number of crosslinkers available, except if there are none available
    // getNFreeSites() gives the number of free sites on both microtubules. Each of those is an (unbiased) option for binding a free linker.
    const bool crosslinkerAvailable = (systemState.getNFreeCrosslinkersOfType(m_typeToBind)!=0);
    m_currentRate = (crosslinkerAvailable ? 1 : 0) * (m_rateOneLinkerToOneSiteTip * systemState.getNFreeSites(MicrotubuleType::FIXED, SiteType::TIP)
                                                    +m_rateOneLinkerToOneSiteBlocked * systemState.getNFreeSites(MicrotubuleType::FIXED, SiteType::BLOCKED);
}

SiteLocation BindFreeCrosslinker::whereToConnect(const SystemState& systemState, RandomGenerator& generator) const
{
    int32_t nFreeSitesTip = systemState.getNFreeSites(MicrotubuleType::FIXED, SiteType::TIP);
    int32_t nFreeSitesBlocked = systemState.getNFreeSites(MicrotubuleType::FIXED, SiteType::BLOCKED);

    #ifdef MYDEBUG
    if(nFreeSitesTip<0 || nFreeSitesBlocked<0 || nFreeSitesTip==0 && nFreeSitesBlocked==0)
    {
        throw GeneralException("BindFreeCrosslinker::whereToConnect() failed to find a proper number of free sites");
    }
    #endif // MYDEBUG

    // Always first bind to fixed when studying transport
    MicrotubuleType microtubuleToConnect = MicrotubuleType::FIXED;

    const double probabilityOnTip = m_rateOneLinkerToOneSiteTip*nFreeSitesTip/(m_rateOneLinkerToOneSiteTip*nFreeSitesTip+m_rateOneLinkerToOneSiteBlocked*nFreeSitesBlocked);
    const SiteType microtubulePartToConnectTo = generator.getBernoulli(probabilityOnTip) ? SiteType::TIP : SiteType::BLOCKED;

    const int32_t nFreeSites = (microtubulePartToConnectTo==SiteType::TIP) ? nFreeSitesTip : nFreeSitesBlocked;

    const int32_t freeSiteLabelToConnect = generator.getUniformInteger(0, nFreeSites-1); // Each site has equal probability of binding

    // Now, we have picked a label to a free site uniformly. Then, map this using a linear (bijective) function to the actual positions on the microtubule where the free sites are
    const int32_t positionToConnectAt = systemState.getFreeSitePosition(microtubuleToConnect, siteType, freeSiteLabelToConnect);

    return SiteLocation{microtubuleToConnect, positionToConnectAt, microtubulePartToConnectTo};
}

void BindFreeCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    SiteLocation connectLocation = whereToConnect(systemState, generator);

    // Binding can be biased towards the head or tail
    Crosslinker::Terminus terminusToConnect = Crosslinker::Terminus::TAIL;

    systemState.connectFreeCrosslinker(m_typeToBind, terminusToConnect, connectLocation);

}


