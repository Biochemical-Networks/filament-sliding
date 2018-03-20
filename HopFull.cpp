#include "HopFull.hpp"

#include <cmath> // exp
#include <vector>
#include <cstddef> // size_t

HopFull::HopFull(const double baseRateHead,
                 const double baseRateTail,
                 const Crosslinker::Type typeToHop,
                 const double springConstant,
                 const double headHopToPlusBiasEnergy,
                 const double tailHopToPlusBiasEnergy)
    :   Reaction(), // Use the base rates instead of the elementaryRate
        m_typeToHop(typeToHop),
        m_springConstant(springConstant),
        m_headHopToPlusBaseRate(baseRateHead*std::exp(headHopToPlusBiasEnergy*0.5)),
        m_headHopToMinusBaseRate(baseRateHead*std::exp(-headHopToPlusBiasEnergy*0.5)),
        m_tailHopToPlusBaseRate(baseRateTail*std::exp(tailHopToPlusBiasEnergy*0.5)),
        m_tailHopToMinusBaseRate(baseRateTail*std::exp(-tailHopToPlusBiasEnergy*0.5))
{
}

HopFull::~HopFull()
{
}

void HopFull::setCurrentRate(const SystemState& systemState)
{
    const std::vector<PossibleFullHop>& possibleFullHops = systemState.getPossibleFullHops(m_typeToHop);

    m_individualRates.clear();
    m_individualRates.reserve(possibleFullHops.size());

    double sum = 0;
    for(const PossibleFullHop& possibleFullHop : possibleFullHops)
    {
        // Two energetic effects influence the rate of hopping:
        // First, there can be a non-equilibrium driving in one direction, causing causing the rate to be biased.
        // Second, the rate depends on the energy of stretching, 0.5*springConstant*extension^2
        // The influence of this energy is spread evenly over the forward and backward reactions,
        // explaining the extra factor 0.5 in the exponent.
        double rate = getBaseRateToHop(possibleFullHop.terminusToHop, possibleFullHop.direction);
        rate*= std::exp(0.25*m_springConstant*
                        (possibleFullHop.oldExtension*possibleFullHop.oldExtension
                         -possibleFullHop.newExtension*possibleFullHop.newExtension));
        m_individualRates.push_back(rate);
        sum += rate;
    }
    m_currentRate = sum;
}

double HopFull::getBaseRateToHop(const Crosslinker::Terminus terminusToHop, const HopDirection directionToHop) const
{
    if(terminusToHop == Crosslinker::Terminus::HEAD && directionToHop == HopDirection::FORWARD)
    {
        return m_headHopToPlusBaseRate;
    }
    else if(terminusToHop == Crosslinker::Terminus::HEAD && directionToHop == HopDirection::BACKWARD)
    {
        return m_headHopToMinusBaseRate;
    }
    else if(terminusToHop == Crosslinker::Terminus::TAIL && directionToHop == HopDirection::FORWARD)
    {
        return m_tailHopToPlusBaseRate;
    }
    else if(terminusToHop == Crosslinker::Terminus::TAIL && directionToHop == HopDirection::BACKWARD)
    {
        return m_tailHopToMinusBaseRate;
    }
    else
    {
        throw GeneralException("HopFull::getBaseRateToHop() was called with improper parameters.");
    }
}

const PossibleFullHop& HopFull::whichHop(const SystemState& systemState, RandomGenerator& generator) const
{
    const std::vector<PossibleFullHop>& possibleFullHops = systemState.getPossibleFullHops(m_typeToHop);

    #ifdef MYDEBUG
    if (possibleFullHops.empty())
    {
        throw GeneralException("HopFull::whichHop() did not have possibilities");
    }
    if(possibleFullHops.size() != m_individualRates.size())
    {
        throw GeneralException("HopFull::whichHop() was called with an outdated vector");
    }
    #endif // MYDEBUG

    // Choose the connection with a probability proportional to its rate
    const double eventIdentifyingRate = generator.getUniform(0,m_currentRate);
    double sum = 0;
    for(std::size_t label = 0; label < m_individualRates.size(); ++label) // We need to find a location, so don't use a range based loop
    {
        sum += m_individualRates.at(label);
        if (sum > eventIdentifyingRate)
        {
            return possibleFullHops.at(label);
        }
    }
    throw GeneralException("The end of HopFull::whichHop() was reached");
}

void HopFull::performReaction(SystemState& systemState, RandomGenerator& generator)
{
    PossibleFullHop hopToMake = whichHop(systemState, generator);
    // A hop consists of disconnecting the crosslinker at one terminus, and then connecting that terminus again
    systemState.disconnectFullyConnectedCrosslinker(*hopToMake.p_fullLinker, hopToMake.terminusToHop);
    systemState.connectPartiallyConnectedCrosslinker(*hopToMake.p_fullLinker, hopToMake.locationToHopTo);
}
