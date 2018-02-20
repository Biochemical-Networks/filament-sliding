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

BindPartialCrosslinker::BindPartialCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind, const double springConstant, const double thermalEnergy)
    :   Reaction(elementaryRate),
        m_typeToBind(typeToBind),
        m_springThermalRatio(springConstant/(4*thermalEnergy))
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
        // m_springThermalRatio = k / (4 k_B T), such that this quantity is only calculat
        const double rate = m_elementaryRate*std::exp(-m_springThermalRatio*possibleConnection.extension*possibleConnection.extension);
        m_individualRates.push_back(rate);
        sum += rate;
    }
    m_currentRate = sum;
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
