#include "UnbindPartialCrosslinker.hpp"

#include "Reaction.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

#include <vector>
#include <utility>

UnbindPartialCrosslinker::UnbindPartialCrosslinker(const double rateOneTerminusDisconnects, const Crosslinker::Type typeToUnbind)
    :   Reaction(),
        m_rateOneTerminusDisconnects(rateOneTerminusDisconnects),
        m_typeToUnbind(typeToUnbind)/*,
        m_headUnbindingFactor(2/(1+std::exp(headBiasEnergy))),
        m_tailUnbindingFactor(2-m_headUnbindingFactor)*/
{
}

UnbindPartialCrosslinker::~UnbindPartialCrosslinker()
{
}

void UnbindPartialCrosslinker::setCurrentRate(const SystemState& systemState)
{
    /*const std::pair<int32_t,int32_t> nPartialsBoundWithHeadAndTail = systemState.getNPartialCrosslinkersBoundWithHeadAndTailOfType(m_typeToUnbind);

    #ifdef MYDEBUG
    int32_t nCrosslinkersOfThisType = systemState.getNPartialCrosslinkersOfType(m_typeToUnbind);
    if (nCrosslinkersOfThisType != nPartialsBoundWithHeadAndTail.first + nPartialsBoundWithHeadAndTail.second)
    {
        throw GeneralException("UnbindPartialCrosslinker::setCurrentRate() found two different numbers of partial linkers");
    }
    #endif // MYDEBUG*/

    m_currentRate = m_rateOneTerminusDisconnects*systemState.getNPartialCrosslinkersOfType(m_typeToUnbind);
}

void UnbindPartialCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    Crosslinker& linkerToDisconnect = whichToDisconnect(systemState, generator);
    systemState.disconnectPartiallyConnectedCrosslinker(linkerToDisconnect);
}

Crosslinker& UnbindPartialCrosslinker::whichToDisconnect(SystemState& systemState, RandomGenerator& generator) const
{
    /*const std::vector<Crosslinker*>& partialsBoundWithHead = systemState.getPartialLinkersBoundWithHead(m_typeToUnbind);*/
    // All partials are bound with only their tail on the fixed microtubule
    const std::vector<Crosslinker*>& partialsBoundWithTail = systemState.getPartialLinkersBoundWithTail(m_typeToUnbind);
/*
    #ifdef MYDEBUG
    if (partialsBoundWithHead.empty() && partialsBoundWithTail.empty())
    {
        throw GeneralException("No partial linker is available to be disconnected in UnbindPartialCrosslinker::whichToDisconnect()");
    }
    #endif // MYDEBUG

    const double probHeadUnbinds = m_rateOneTerminusDisconnects*partialsBoundWithHead.size()*m_headUnbindingFactor/m_currentRate;

    // Choose from which of the two sets (bound with head or tail) the crosslinker needs to be picked.
    const std::vector<Crosslinker*>& whichSet = (generator.getBernoulli(probHeadUnbinds))?partialsBoundWithHead:partialsBoundWithTail;*/

    // Then, generate the position in the set of those linkers uniformly. This way, two random numbers are used, but it does prevent a loop through all separate possible unbinding events.
    const int32_t label = generator.getUniformInteger(0, partialsBoundWithTail.size()-1);
    return *partialsBoundWithTail.at(label);
}
