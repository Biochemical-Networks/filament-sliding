#include "UnbindPartialCrosslinker.hpp"

#include "Reaction.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

#include <vector>

UnbindPartialCrosslinker::UnbindPartialCrosslinker(const double elementaryRate, const Crosslinker::Type typeToUnbind)
    :   Reaction(elementaryRate),
        m_typeToUnbind(typeToUnbind)
{
}

UnbindPartialCrosslinker::~UnbindPartialCrosslinker()
{
}

void UnbindPartialCrosslinker::setCurrentRate(const SystemState& systemState)
{
    int32_t nCrosslinkersOfThisType = systemState.getNPartialCrosslinkersOfType(m_typeToUnbind);
    m_currentRate = m_elementaryRate * nCrosslinkersOfThisType;
}

void UnbindPartialCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    Crosslinker& linkerToDisconnect = whichToDisconnect(systemState, generator);
    systemState.disconnectPartiallyConnectedCrosslinker(linkerToDisconnect);
}

Crosslinker& UnbindPartialCrosslinker::whichToDisconnect(SystemState& systemState, RandomGenerator& generator) const
{
    const std::vector<Crosslinker*>& partials = systemState.getPartialLinkers(m_typeToUnbind);
    if (partials.empty())
    {
        throw GeneralException("No partial linker is available to be disconnected in UnbindPartialCrosslinker::whichToDisconnect()");
    }
    int32_t label = generator.getUniformInteger(0, partials.size()-1);
    return *partials.at(label);
}
