#include "Initialiser.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "GeneralException/GeneralException.hpp"

#include "Crosslinker.hpp"
#include "Extremity.hpp"

#include <string>
#include <cstdint>
#include <numeric> // std::iota
#include <algorithm> // std::shuffle, std::max_element, std::min
#include <cmath> // std::ceil
#include <vector>
#include <iterator> // std::distance

Initialiser::Initialiser(const double initialPositionMicrotubule, const double fractionOverlapSitesConnected, const std::string initialCrosslinkerDistributionString)
    :   m_initialPositionMicrotubule(initialPositionMicrotubule),
        m_fractionOverlapSitesConnected(fractionOverlapSitesConnected)
{
    // Translate the string to an enum object Initialiser::InitialCrosslinkerDistribution
    // switch statements do not work with strings

    if (initialCrosslinkerDistributionString=="RANDOM")
    {
        m_initialCrosslinkerDistribution = InitialCrosslinkerDistribution::RANDOM;
    }
    else if (initialCrosslinkerDistributionString=="HEADSMOBILE")
    {
        m_initialCrosslinkerDistribution = InitialCrosslinkerDistribution::HEADSMOBILE;
    }
    else if (initialCrosslinkerDistributionString=="TAILSMOBILE")
    {
        m_initialCrosslinkerDistribution = InitialCrosslinkerDistribution::TAILSMOBILE;
    }
    else if (initialCrosslinkerDistributionString=="TEST")
    {
        m_initialCrosslinkerDistribution = InitialCrosslinkerDistribution::TEST;
    }
    else
    {
        throw GeneralException("In the Initialiser constructor, the given initialCrosslinkerDistributionString does not hold a recognised value.");
    }
}

Initialiser::~Initialiser()
{
}

void Initialiser::initialise(SystemState& systemState, RandomGenerator& generator)
{
    systemState.setMicrotubulePosition(m_initialPositionMicrotubule);

    initialiseCrosslinkers(systemState, generator);

}

void Initialiser::initialiseCrosslinkers(SystemState& systemState, RandomGenerator& generator)
{
    const int32_t nSitesOverlapFixed = systemState.getNSitesOverlapFixed();
    const int32_t nSitesOverlapMobile = systemState.getNSitesOverlapMobile();
    // std::cout << "nSitesOverlapFixed: " << nSitesOverlapFixed << " nSitesOverlapMobile: " << nSitesOverlapMobile << '\n';

    // nSitesOverlapFixed and nSitesOverlapMobile can be different when one microtubule is completely overlapping with the other, then use the shortest
    const int32_t nSitesOverlap = std::min(nSitesOverlapFixed, nSitesOverlapMobile);

    std::vector<int32_t> positionsToConnect(nSitesOverlap);
    std::iota(positionsToConnect.begin(), positionsToConnect.end(), 0); // Fill vector with 0, 1, ..., n-1, such that you have a list of all positions
    switch(m_initialCrosslinkerDistribution)
    {
        case InitialCrosslinkerDistribution::RANDOM:
        case InitialCrosslinkerDistribution::HEADSMOBILE:
        case InitialCrosslinkerDistribution::TAILSMOBILE:
            std::shuffle(positionsToConnect.begin(), positionsToConnect.end(), generator.getBareGenerator()); // Shuffle the positions
            break;
        case InitialCrosslinkerDistribution::TEST:
            // Don't shuffle: linkers get connected from the beginning, such that the initial distribution is deterministic
            // Passives are connected first, then duals, then actives
            break;
        default:
            throw GeneralException("Caller of Initialiser::initialiseCrosslinkers() asked for an unsupported initial crosslinker distribution");
            break;
    }

    // The following takes the ceiling, since we would always like to have at least one crosslinker to connect in case there is a finite m_fractionOverlapSitesConnected
    int32_t nSitesToConnect = static_cast<int32_t> (std::ceil(m_fractionOverlapSitesConnected * nSitesOverlap));

    if (nSitesToConnect > nSitesOverlap) // Make sure nothing went wrong in the conversion from double to int
    {
        nSitesToConnect = nSitesOverlap;
    }

    int32_t nFreeCrosslinkers = systemState.getNFreeCrosslinkers();

    try
    {
        if (nFreeCrosslinkers < nSitesToConnect)
        {
            std::string numberOfFreeCrosslinkers = std::to_string(nFreeCrosslinkers);
            std::string desiredNumberOfCrosslinkers = std::to_string(nSitesToConnect);
            throw GeneralException("In Initialiser::initialiseCrosslinkers(), the number of free crosslinkers is "
                                   +numberOfFreeCrosslinkers
                                   +", lower than the initial number of crosslinkers needed, "
                                   +desiredNumberOfCrosslinkers
                                   +", for achieving a well connected system. "
                                   +"\nContinue with the actual number of crosslinkers.");
        }
    }
    catch(GeneralException)
    {
        nSitesToConnect = nFreeCrosslinkers; // Set the number of crosslinkers to the maximum number available.
    }

    /* The following code will calculate the number of crosslinkers of each type to be connected.
     * Declare the numbers of crosslinkers of each type, such that they can be passed by reference to a function that will calculate them.
     */
    int32_t nPassiveCrosslinkersToConnect;
    int32_t nDualCrosslinkersToConnect;
    int32_t nActiveCrosslinkersToConnect;
    nCrosslinkersEachTypeToConnect(nPassiveCrosslinkersToConnect,
                                   nDualCrosslinkersToConnect,
                                   nActiveCrosslinkersToConnect,
                                   nSitesToConnect,
                                   nFreeCrosslinkers,
                                   systemState.getNFreeCrosslinkersOfType(Crosslinker::Type::PASSIVE),
                                   systemState.getNFreeCrosslinkersOfType(Crosslinker::Type::DUAL),
                                   systemState.getNFreeCrosslinkersOfType(Crosslinker::Type::ACTIVE));

    /* The following code will connect the crosslinkers */

    int32_t connectedSoFar = 0;

    // Get the following numbers locally, such that they don't need to be retrieved upon every entry in the following loops
    int32_t firstSiteOverlapFixed = systemState.firstSiteOverlapFixed();
    int32_t firstSiteOverlapMobile = systemState.firstSiteOverlapMobile();

    // Crosslinkers will usually be initialised in a highly stretched state;
    // The first site in the overlap of the mobile microtubule can be a distance maxStretch from that of the fixed microtubule.
    // Therefore, the program should be allowed to run at least for a little while for the system to assume an equilibrium distribution.
    // The crosslinkers will never cross each other, since they are ordered in the same way on the fixed and mobile microtubules
    for (int32_t i = 0; i<nPassiveCrosslinkersToConnect; ++connectedSoFar, ++i)
    {
        systemState.fullyConnectFreeCrosslinker(Crosslinker::Type::PASSIVE,
                                              terminusToConnectToFixedMicrotubule(generator),
                                              firstSiteOverlapFixed+positionsToConnect.at(connectedSoFar), // The position on the fixed microtubule
                                              firstSiteOverlapMobile+positionsToConnect.at(connectedSoFar));
    }

    for (int32_t i = 0; i<nDualCrosslinkersToConnect; ++connectedSoFar, ++i)
    {
        systemState.fullyConnectFreeCrosslinker(Crosslinker::Type::DUAL,
                                              terminusToConnectToFixedMicrotubule(generator),
                                              firstSiteOverlapFixed+positionsToConnect.at(connectedSoFar), // The position on the fixed microtubule
                                              firstSiteOverlapMobile+positionsToConnect.at(connectedSoFar));
    }

    for (int32_t i = 0; i<nActiveCrosslinkersToConnect; ++connectedSoFar, ++i)
    {
        systemState.fullyConnectFreeCrosslinker(Crosslinker::Type::ACTIVE,
                                              terminusToConnectToFixedMicrotubule(generator),
                                              firstSiteOverlapFixed+positionsToConnect.at(connectedSoFar), // The position on the fixed microtubule
                                              firstSiteOverlapMobile+positionsToConnect.at(connectedSoFar));
    }

    #ifdef MYDEBUG
    if (connectedSoFar != nSitesToConnect)
    {
        throw GeneralException("Something went wrong in Initialiser::initialiseCrosslinkers(), the number of connected crosslinkers failed to equal the number of sites to connect");
    }
    #endif // MYDEBUG

}

void Initialiser::nCrosslinkersEachTypeToConnect(int32_t& nPassiveCrosslinkersToConnect,
                                                 int32_t& nDualCrosslinkersToConnect,
                                                 int32_t& nActiveCrosslinkersToConnect,
                                                 const int32_t nSitesToConnect,
                                                 const int32_t nFreeCrosslinkers,
                                                 const int32_t nFreePassiveCrosslinkers,
                                                 const int32_t nFreeDualCrosslinkers,
                                                 const int32_t nFreeActiveCrosslinkers) const
{
    // To calculate the number of connected crosslinkers to assign to each type,
    // first use integer division. In this setup, e.g. 0 <= remainderPassiveCrosslinkers < nFreeCrosslinkers.
    // Since the fractions are floored to integers, the resulting number of connected crosslinkers is always lower than or equal to nSitesToConnect.
    // To make up for the loss, assign the remaining crosslinkers to the largest remainders.
    // But first, check whether the user is trying to have no crosslinkers in the system.

    if(nFreeCrosslinkers==0 && nSitesToConnect==0)
    {
        nPassiveCrosslinkersToConnect=nDualCrosslinkersToConnect=nActiveCrosslinkersToConnect=0;
        return;
    }
    #ifdef MYDEBUG
    if(nFreeCrosslinkers==0 && nSitesToConnect!=0)
    {
        throw GeneralException("The user of Initialiser::nCrosslinkersEachTypeToConnect() tried to connect crosslinkers, but none where available.");
    }
    #endif // MYDEBUG

    // Here, we can assume that nFreeCrosslinkers>0

    nPassiveCrosslinkersToConnect = (nFreePassiveCrosslinkers*nSitesToConnect) / nFreeCrosslinkers;
    int32_t remainderPassiveCrosslinkers = (nFreePassiveCrosslinkers*nSitesToConnect) % nFreeCrosslinkers;

    nDualCrosslinkersToConnect = (nFreeDualCrosslinkers*nSitesToConnect) / nFreeCrosslinkers;
    int32_t remainderDualCrosslinkers = (nFreeDualCrosslinkers*nSitesToConnect) % nFreeCrosslinkers;

    nActiveCrosslinkersToConnect = (nFreeActiveCrosslinkers*nSitesToConnect) / nFreeCrosslinkers;
    int32_t remainderActiveCrosslinkers = (nFreeActiveCrosslinkers*nSitesToConnect) % nFreeCrosslinkers;

    std::vector<int32_t> remainders{remainderPassiveCrosslinkers, remainderDualCrosslinkers, remainderActiveCrosslinkers};

    // Make sure that the total number of crosslinkers to connect equals the number of sites to connect to.
    // Add a crosslinker to the type that has the highest remainder after the integer division above.
    while (nPassiveCrosslinkersToConnect+nDualCrosslinkersToConnect+nActiveCrosslinkersToConnect < nSitesToConnect)
    {
        std::vector<int32_t>::iterator iteratorMaxRemainder;
        iteratorMaxRemainder = std::max_element(remainders.begin(), remainders.end());
        *iteratorMaxRemainder -= nFreeCrosslinkers; // remainder<nFreeCrosslinkers, so this makes the remainder negative. Hence, the same point won't be chosen again.
        int32_t positionMaxRemainder = std::distance(remainders.begin(), iteratorMaxRemainder);

        switch (positionMaxRemainder) // The possible values are 0 (passive), 1 (dual), and 2 (active)
        {
            case 0:
                ++nPassiveCrosslinkersToConnect;
                break;
            case 1:
                ++nDualCrosslinkersToConnect;
                break;
            case 2:
                ++nActiveCrosslinkersToConnect;
                break;
            default:
                throw GeneralException("The iterator in Initialiser::nCrosslinkersEachTypeToConnect() returned a nonexistent position");
                break;
        }
    }
}

Crosslinker::Terminus Initialiser::terminusToConnectToFixedMicrotubule(RandomGenerator &generator)
{
    switch(m_initialCrosslinkerDistribution)
    {
        case InitialCrosslinkerDistribution::RANDOM:
            return ((generator.getBernoulli(0.5))?(Crosslinker::Terminus::HEAD):(Crosslinker::Terminus::TAIL));
            break;
        case InitialCrosslinkerDistribution::HEADSMOBILE:
            return Crosslinker::Terminus::TAIL;
            break;
        case InitialCrosslinkerDistribution::TAILSMOBILE:
            return Crosslinker::Terminus::HEAD;
            break;
        case InitialCrosslinkerDistribution::TEST:
            return Crosslinker::Terminus::TAIL;
        default:
            throw GeneralException("Caller of Initialiser::terminusToConnectToFixedMicrotubule() asked for an unsupported initial crosslinker distribution");
            break;
    }
}
