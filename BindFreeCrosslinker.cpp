#include "BindFreeCrosslinker.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Extremity.hpp"

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
    int32_t nCrosslinkersOfType = systemState.getNFreeCrosslinkersOfType(m_typeToBind);
    m_currentRate = m_elementaryRate * systemState.getNFreeSites() * nCrosslinkersOfType;
}

void BindFreeCrosslinker::updateAccumulatedAction(const SystemState& systemState)
{
    m_accumulatedAction += m_currentRate;
}

void BindFreeCrosslinker::whereToConnect(const SystemState& systemState, RandomGenerator& generator, Extremity::MicrotubuleType& microtubuleToConnect, int32_t& position)
{
    int32_t nFreeSitesFixed = systemState.getNFreeSitesFixed();
    int32_t nFreeSitesMobile = systemState.getNFreeSitesMobile();
    int32_t nFreeSites = nFreeSitesFixed + nFreeSitesMobile;

    // Initialise these as if the fixed microtubule needs to be connected, and change it after if the opposite needs to happen
    microtubuleToConnect = Extremity::MicrotubuleType::FIXED;
    int32_t freeSiteLabelToConnect = generator.getUniformInteger(0, nFreeSites-1);
    if (freeSiteLabelToConnect >= nFreeSitesFixed)
    {
        microtubuleToConnect = Extremity::MicrotubuleType::MOBILE;
        freeSiteLabelToConnect -= nFreeSitesFixed;
    }

    position = systemState.getFreeSitePosition(microtubuleToConnect, freeSiteLabelToConnect);
}

void BindFreeCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    Extremity::MicrotubuleType microtubuleToConnect;
    int32_t position;
    whereToConnect(systemState, generator, microtubuleToConnect, position); // Get the microtubule and site which should be connected.

    // The following can be changed when there are different binding affinities for the head and tail of a crosslinker.
    // Now, the binding affinities are assumed to be equal.
    Crosslinker::Terminus terminusToConnect = ((generator.getBernoulli(0.5))?(Crosslinker::Terminus::HEAD):(Crosslinker::Terminus::TAIL));

    systemState.connectFreeCrosslinker(m_typeToBind, terminusToConnect, microtubuleToConnect, position);


}


