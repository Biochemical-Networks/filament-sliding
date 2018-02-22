#include "UnbindFullCrosslinker.hpp"

#include "FullConnection.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"

#include <vector>

UnbindFullCrosslinker::UnbindFullCrosslinker(const double elementaryRate, const Crosslinker::Type typeToUnbind, const double springConstant)
    :   Reaction(elementaryRate),
        m_typeToUnbind(typeToUnbind),
        m_springConstant(springConstant)
{
}

UnbindFullCrosslinker::~UnbindFullCrosslinker()
{
}

void UnbindFullCrosslinker::setCurrentRate(const SystemState& systemState)
{
    const std::vector<FullConnection>& fullConnections = systemState.getFullConnections(m_typeToUnbind);
    m_individualRates.clear();
    m_individualRates.reserve(fullConnections.size());
    double sum = 0;
    for(const FullConnection& fullConnection : fullConnections)
    {
        // spread the effect of extension evenly over connecting and disconnecting: rate scales with exp(k x^2 / (4 k_B T))
        const double rate = m_elementaryRate*std::exp(-m_springConstant*fullConnection.extension*fullConnection.extension*0.25);
        m_individualRates.push_back(rate);
        sum += rate;
    }
    m_currentRate = sum;
}

void UnbindFullCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    FullConnection connectionToDisconnect = whichToDisconnect(systemState, generator);

    // The following can be changed when there are different binding affinities for the head and tail of a crosslinker.
    // Now, the binding affinities are assumed to be equal.
    double probHeadUnbinds = 0.5;
    Crosslinker::Terminus terminusToDisconnect = ((generator.getBernoulli(probHeadUnbinds))?(Crosslinker::Terminus::HEAD):(Crosslinker::Terminus::TAIL));

    systemState.disconnectFullyConnectedCrosslinker(*connectionToDisconnect.p_fullLinker, terminusToDisconnect);
}

FullConnection UnbindFullCrosslinker::whichToDisconnect(SystemState& systemState, RandomGenerator& generator) const
{
    const std::vector<FullConnection>& fullConnections = systemState.getFullConnections(m_typeToUnbind);

    if (fullConnections.empty())
    {
        throw GeneralException("UnbindFullCrosslinker::whichToDisconnect() was not able to disconnect a linker");
    }
    if(fullConnections.size() != m_individualRates.size())
    {
        throw GeneralException("UnbindFullCrosslinker::whichToDisconnect() was called with an outdated vector");
    }

    // Choose the connection with a probability proportional to its rate
    const double eventIdentifyingRate = generator.getUniform(0,m_currentRate);
    double sum = 0;
    for(std::size_t label = 0; label < m_individualRates.size(); ++label) // We need to find a location, so don't use a range based loop
    {
        sum += m_individualRates.at(label);
        if (sum > eventIdentifyingRate)
        {
            return fullConnections.at(label);
        }
    }
    throw GeneralException("The end of UnbindFullCrosslinker::whichToConnect() was reached");
}