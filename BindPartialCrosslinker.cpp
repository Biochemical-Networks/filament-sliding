#include "BindPartialCrosslinker.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleType.hpp"

#include "CrosslinkerContainer.hpp"
#include "Crosslinker.hpp"

#include <cstdint>

BindPartialCrosslinker::BindPartialCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind)
    :   Reaction(elementaryRate),
        m_typeToBind(typeToBind)
{
}

BindPartialCrosslinker::~BindPartialCrosslinker()
{
}


void BindPartialCrosslinker::setCurrentRate(const SystemState& systemState)
{

}


SiteLocation BindPartialCrosslinker::whereToConnect(const SystemState& systemState, RandomGenerator& generator)
{
    /* This function will have to seek a spot to connect the crosslinkers. For a partial crosslinker to be able to connect, it needs to have at least one opposite site free,
     * closer than one SystemState.getMaxStretch(). If there is no possible site, the rate of this reaction to be called should be zero.
     */
}


void BindPartialCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{

}
