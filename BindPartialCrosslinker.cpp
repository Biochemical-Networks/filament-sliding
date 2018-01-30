#include "BindPartialCrosslinker.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Extremity.hpp"

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


// Return through references, since two values are returned
void BindPartialCrosslinker::whereToConnect(const SystemState& systemState, RandomGenerator& generator, Extremity::MicrotubuleType& microtubuleToConnect, int32_t& position)
{
    /* This function will have to seek a spot to connect the crosslinkers. For a partial crosslinker to be able to connect, it needs to have at least one opposite site free,
     * closer than one SystemState.getMaxStretch(). If there is no possible site, the rate of this reaction to be called should be zero.
     */
}


void BindPartialCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{

}
