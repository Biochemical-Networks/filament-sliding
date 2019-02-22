#include "BindFreeCrosslinker.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleType.hpp"

#include <cstdint>
#include <cmath>

BindFreeCrosslinker::BindFreeCrosslinker(const double rateOneLinkerToOneSite, const Crosslinker::Type typeToBind/*, const double headBiasEnergy*/)
    :   Reaction(),
        m_rateOneLinkerToOneSite(rateOneLinkerToOneSite),
        m_typeToBind(typeToBind)/*,
        m_probHeadBinds(1/(1+std::exp(-headBiasEnergy))) // headBiasEnergy should have units of (k_B T)*/
{
}

BindFreeCrosslinker::~BindFreeCrosslinker()
{
}

void BindFreeCrosslinker::setCurrentRate(const SystemState& systemState)
{
    // getNFreeSites() gives the number of free sites on both microtubules. Each of those is an (unbiased) option for binding a free linker.
    int32_t nCrosslinkersOfThisType = systemState.getNFreeCrosslinkersOfType(m_typeToBind);
    m_currentRate = m_rateOneLinkerToOneSite * systemState.getNFreeSitesFixed() * nCrosslinkersOfThisType;
}

SiteLocation BindFreeCrosslinker::whereToConnect(const SystemState& systemState, RandomGenerator& generator) const
{
    int32_t nFreeSitesFixed = systemState.getNFreeSitesFixed();
    /*int32_t nFreeSitesMobile = systemState.getNFreeSitesMobile();
    int32_t nFreeSites = nFreeSitesFixed + nFreeSitesMobile;*/

    // Always first bind to fixed when studying transport
    MicrotubuleType microtubuleToConnect = MicrotubuleType::FIXED;
    int32_t freeSiteLabelToConnect = generator.getUniformInteger(0, nFreeSitesFixed-1); // Each site has equal probability of binding

    // Now, we have picked a label to a free site uniformly. Then, map this using a linear (bijective) function to the actual positions on the microtubule where the free sites are
    int32_t positionToConnectAt = systemState.getFreeSitePosition(microtubuleToConnect, freeSiteLabelToConnect);

    return SiteLocation{microtubuleToConnect, positionToConnectAt};
}

void BindFreeCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    SiteLocation connectLocation = whereToConnect(systemState, generator);

    // Binding can be biased towards the head or tail
    Crosslinker::Terminus terminusToConnect = Crosslinker::Terminus::TAIL;

    systemState.connectFreeCrosslinker(m_typeToBind, terminusToConnect, connectLocation);

}


