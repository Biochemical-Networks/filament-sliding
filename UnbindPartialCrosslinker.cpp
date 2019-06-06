#include "UnbindPartialCrosslinker.hpp"

#include "Reaction.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

#include <vector>
#include <utility>

UnbindPartialCrosslinker::UnbindPartialCrosslinker(const double rateOneTerminusDisconnectsTip, const double rateOneTerminusDisconnectsBlocked, const Crosslinker::Type typeToUnbind)
    :   Reaction(),
        m_rateOneTerminusDisconnectsTip(rateOneTerminusDisconnectsTip),
        m_rateOneTerminusDisconnectsBlocked(rateOneTerminusDisconnectsBlocked),
        m_typeToUnbind(typeToUnbind)
{
}

UnbindPartialCrosslinker::~UnbindPartialCrosslinker()
{
}

void UnbindPartialCrosslinker::setCurrentRate(const SystemState& systemState)
{
    m_currentRate = m_rateOneTerminusDisconnectsTip*systemState.getNPartialCrosslinkersOfType(m_typeToUnbind, SiteType::TIP)
                    + m_rateOneTerminusDisconnectsBlocked*systemState.getNPartialCrosslinkersOfType(m_typeToUnbind, SiteType::BLOCKED);
}

void UnbindPartialCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    Crosslinker& linkerToDisconnect = whichToDisconnect(systemState, generator);
    systemState.disconnectPartiallyConnectedCrosslinker(linkerToDisconnect);
}

Crosslinker& UnbindPartialCrosslinker::whichToDisconnect(SystemState& systemState, RandomGenerator& generator) const
{
    // All partials are bound with only their tail on the fixed microtubule
    const std::vector<Crosslinker*>& partialsBoundOnTip = systemState.getPartialLinkers(m_typeToUnbind, SiteType::TIP);
    const std::vector<Crosslinker*>& partialsBoundOnBlocked = systemState.getPartialLinkers(m_typeToUnbind, SiteType::BLOCKED);

    #ifdef MYDEBUG
    if (partialsBoundOnTip.empty() && partialsBoundOnBlocked.empty())
    {
        throw GeneralException("No partial linker is available to be disconnected in UnbindPartialCrosslinker::whichToDisconnect()");
    }
    if (partialsBoundOnTip.size()!=systemState.getNPartialCrosslinkersOfType(m_typeToUnbind, SiteType::TIP) ||
        partialsBoundOnBlocked.size()!=systemState.getNPartialCrosslinkersOfType(m_typeToUnbind, SiteType::BLOCKED) )
    {
        throw GeneralException("UnbindPartialCrosslinker::whichToDisconnect() saw two numbers of partials, something went wrong.");
    }
    #endif // MYDEBUG

    // m_currentRate must be nonzero, otherwise this function is never called
    const double probUnbindsFromTip = m_rateOneTerminusDisconnectsTip*partialsBoundOnTip.size()/m_currentRate;

    #ifdef MYDEBUG
    if (probUnbindsFromTip>1)
    {
        throw GeneralException("Wrong probability calculated in UnbindPartialCrosslinker::whichToDisconnect()");
    }
    #endif // MYDEBUG

    // Choose from which of the two sets (bound with head or tail) the crosslinker needs to be picked.
    const SiteType microtubulePartToDisconnectFrom = (generator.getBernoulli(probUnbindsFromTip)) ? SiteType::TIP : SiteType::BLOCKED;

    // Then, generate the position in the set of those linkers uniformly. This way, two random numbers are used, but it does prevent a loop through all separate possible unbinding events.
    switch(microtubulePartToDisconnectFrom)
    {
    case SiteType::TIP:
        return *partialsBoundOnTip.at(generator.getUniformInteger(0, partialsBoundOnTip.size()-1));
    case SiteType::BLOCKED:
        return *partialsBoundOnBlocked.at(generator.getUniformInteger(0, partialsBoundOnBlocked.size()-1));
    default:
        throw GeneralException("UnbindPartialCrosslinker::whichToDisconnect() created a nonexisting SiteType to unbind from.");
    }
}
