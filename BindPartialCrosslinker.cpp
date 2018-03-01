#include "BindPartialCrosslinker.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "MicrotubuleType.hpp"
#include "PossibleFullConnection.hpp"

#include "CrosslinkerContainer.hpp"
#include "Crosslinker.hpp"

#include <cstdint>
#include <vector>
#include <cmath> // exp
#include <cstddef> // size_t

BindPartialCrosslinker::BindPartialCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind, const double springConstant)
    :   Reaction(elementaryRate),
        m_typeToBind(typeToBind),
        m_springConstant(springConstant)
{
}

BindPartialCrosslinker::~BindPartialCrosslinker()
{
}

// Energy dependent rate
void BindPartialCrosslinker::setCurrentRate(const SystemState& systemState)
{
    const std::vector<PossibleFullConnection>& possibleConnections = systemState.getPossibleConnections(m_typeToBind);
    m_individualRates.clear();
    m_individualRates.reserve(possibleConnections.size());

    double sum = 0;
    for(const PossibleFullConnection& possibleConnection : possibleConnections)
    {
        // spread the effect of extension evenly over connecting and disconnecting: rate scales with exp(-k x^2 / (4 k_B T))
        const double rate = m_elementaryRate*std::exp(-m_springConstant*possibleConnection.extension*possibleConnection.extension*0.25);
        m_individualRates.push_back(rate);
        sum += rate;
    }
    m_currentRate = sum;
}

// This function uses the current (individual) rates, make sure they are updated!
PossibleFullConnection BindPartialCrosslinker::whichToConnect(const SystemState& systemState, RandomGenerator& generator) const
{
    const std::vector<PossibleFullConnection>& possibleConnections = systemState.getPossibleConnections(m_typeToBind);

    if (possibleConnections.empty())
    {
        throw GeneralException("BindPartialCrosslinker::whichToConnect() was not able to disconnect a linker");
    }
    if(possibleConnections.size() != m_individualRates.size())
    {
        throw GeneralException("BindPartialCrosslinker::whichToConnect() was called with an outdated vector");
    }

    // Choose the connection with a probability proportional to its rate
    const double eventIdentifyingRate = generator.getUniform(0,m_currentRate);
    double sum = 0;
    for(std::size_t label = 0; label < m_individualRates.size(); ++label) // We need to find a location, so don't use a range based loop
    {
        sum += m_individualRates.at(label);
        if (sum > eventIdentifyingRate)
        {
            return possibleConnections.at(label);
        }
    }
    throw GeneralException("The end of BindPartialCrosslinker::whichToConnect() was reached");
}


void BindPartialCrosslinker::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    PossibleFullConnection connectionToMake = whichToConnect(systemState, generator);

    systemState.connectPartiallyConnectedCrosslinker(*(connectionToMake.p_partialLinker), connectionToMake.location);

}
