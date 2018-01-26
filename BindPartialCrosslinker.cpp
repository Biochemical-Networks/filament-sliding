#include "BindPartialCrosslinker.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Extremity.hpp"

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

}


void BindPartialCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{

}
