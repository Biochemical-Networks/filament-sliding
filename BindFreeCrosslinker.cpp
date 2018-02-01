#include "BindFreeCrosslinker.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleType.hpp"

#include <cstdint>

BindFreeCrosslinker::BindFreeCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind)
    :   Reaction(elementaryRate),
        m_typeToBind(typeToBind)
{
}

BindFreeCrosslinker::~BindFreeCrosslinker()
{
}

void BindFreeCrosslinker::setCurrentRate(const SystemState& systemState)
{
    // getNFreeSites() gives the number of free sites on both microtubules. A fully connected crosslinker takes two of those sites.
    int32_t nCrosslinkersOfThisType = systemState.getNFreeCrosslinkersOfType(m_typeToBind);
    m_currentRate = m_elementaryRate * systemState.getNFreeSites() * nCrosslinkersOfThisType;
}


SiteLocation BindFreeCrosslinker::whereToConnect(const SystemState& systemState, RandomGenerator& generator)
{
    int32_t nFreeSitesFixed = systemState.getNFreeSitesFixed();
    int32_t nFreeSitesMobile = systemState.getNFreeSitesMobile();
    int32_t nFreeSites = nFreeSitesFixed + nFreeSitesMobile;

    // Initialise these as if the fixed microtubule needs to be connected, and change it after if the opposite needs to happen
    MicrotubuleType microtubuleToConnect = MicrotubuleType::FIXED;
    int32_t freeSiteLabelToConnect = generator.getUniformInteger(0, nFreeSites-1);
    if (freeSiteLabelToConnect >= nFreeSitesFixed)
    {
        microtubuleToConnect = MicrotubuleType::MOBILE;
        freeSiteLabelToConnect -= nFreeSitesFixed;
    }

    int32_t positionToConnectAt = systemState.getFreeSitePosition(microtubuleToConnect, freeSiteLabelToConnect);

    return SiteLocation{microtubuleToConnect, positionToConnectAt};
}

void BindFreeCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    SiteLocation connectLocation = whereToConnect(systemState, generator);

    // The following can be changed when there are different binding affinities for the head and tail of a crosslinker.
    // Now, the binding affinities are assumed to be equal.
    double probHeadBinds = 0.5;
    Crosslinker::Terminus terminusToConnect = ((generator.getBernoulli(probHeadBinds))?(Crosslinker::Terminus::HEAD):(Crosslinker::Terminus::TAIL));

    systemState.connectFreeCrosslinker(m_typeToBind, terminusToConnect, connectLocation);

}


