#include "Initialiser.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "GeneralException/GeneralException.hpp"

#include "Crosslinker.hpp"
#include "Extremity.hpp"


Initialiser::Initialiser(const double initialPositionMicrotubule, const double fractionConnectedCrosslinkers, const std::string initialCrosslinkerDistributionString)
    :   m_initialPositionMicrotubule(initialPositionMicrotubule),
        m_fractionConnectedCrosslinkers(fractionConnectedCrosslinkers)
{
    // Translate the string to an enum object Initialiser::InitialCrosslinkerDistribution

    if (initialCrosslinkerDistributionString=="RANDOM")
    {
        m_initialCrosslinkerDistribution = Initialiser::InitialCrosslinkerDistribution::RANDOM;
    }
    else if (initialCrosslinkerDistributionString=="HEADSMOBILE")
    {
        m_initialCrosslinkerDistribution = Initialiser::InitialCrosslinkerDistribution::HEADSMOBILE;
    }
    else if (initialCrosslinkerDistributionString=="TAILSMOBILE")
    {
        m_initialCrosslinkerDistribution = Initialiser::InitialCrosslinkerDistribution::TAILSMOBILE;
    }
    else if (initialCrosslinkerDistributionString=="ALLCONNECTED")
    {
        m_initialCrosslinkerDistribution = Initialiser::InitialCrosslinkerDistribution::ALLCONNECTED;
    }
    else
    {
        throw GeneralException("The given initialCrosslinkerDistributionString does not hold a recognised value.");
    }
}

Initialiser::~Initialiser()
{
}

void Initialiser::initialise(SystemState& systemState, RandomGenerator& generator)
{
    systemState.setMicrotubulePosition(m_initialPositionMicrotubule);


    systemState.connectFreeCrosslinker(Crosslinker::Type::PASSIVE, Crosslinker::Terminus::HEAD, Extremity::MicrotubuleType::FIXED, 0); // TEST
}
