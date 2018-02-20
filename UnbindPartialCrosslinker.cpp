#include "UnbindPartialCrosslinker.hpp"

#include "Reaction.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

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

}
