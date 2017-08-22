#include "Initialiser.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "GeneralException/GeneralException.hpp"

#include "Crosslinker.hpp"
#include "Extremity.hpp"

#include <cstdint>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <vector>


Initialiser::Initialiser(const double initialPositionMicrotubule, const double fractionOverlapSitesConnected, const std::string initialCrosslinkerDistributionString)
    :   m_initialPositionMicrotubule(initialPositionMicrotubule),
        m_fractionOverlapSitesConnected(fractionOverlapSitesConnected)
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

void Initialiser::initialiseCrosslinkersRandom(SystemState& systemState, RandomGenerator& generator)
{
    int32_t nSitesOverlapFixed = systemState.getNSitesOverlapFixed();
    int32_t nSitesOverlapMobile = systemState.getNSitesOverlapMobile();

    int32_t nSitesOverlap; // If Fixed and Mobile are equal, then use that number. Can be different when one microtubule is completely overlapping with the other, then use the shortest
    (nSitesOverlapFixed < nSitesOverlapMobile) ? (nSitesOverlap = nSitesOverlapFixed) : (nSitesOverlap = nSitesOverlapMobile);

    std::vector<int32_t> positionsToConnect(nSitesOverlap);
    std::iota(positionsToConnect.begin(), positionsToConnect.end(), 0); // Fill vector with 0, 1, ..., n-1, such that you have a list of all positions
    std::shuffle(positionsToConnect.begin(), positionsToConnect.end(), generator.getBareGenerator()); // Shuffle the positions

    int32_t nSitesToConnect = static_cast<int32_t> (std::ceil(m_fractionOverlapSitesConnected * nSitesOverlap));
    if (nSitesToConnect > nSitesOverlap) // Check whether nothing went wrong in the conversion from double to int
    {
        nSitesToConnect = nSitesOverlap;
    }

    int32_t nFreeCrosslinkers = systemState.getNFreeCrosslinkers();

    try
    {
        if (nFreeCrosslinkers < nSitesToConnect)
        {
            throw GeneralException("The number of free crosslinkers is lower than the initial number of crosslinkers needed for achieving a well connected system. \nContinue with the actual number of crosslinkers.");
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
                                   systemState.getNFreePassiveCrosslinkers(),
                                   systemState.getNFreeDualCrosslinkers(),
                                   systemState.getNFreeActiveCrosslinkers());

    /* The following code will connect the crosslinkers */

    int32_t connectedSoFar = 0;

/*    for (int32_t i = 0; i<nPassiveCrosslinkersToConnect; ++connectedSoFar, ++i)
    {
        systemState.connectFreeCrosslinker(Crosslinker::Type::PASSIVE, (generator.getBernoulli())?(Crosslinker::Terminus::HEAD):(Crosslinker::Terminus::TAIL), )
        connectFreeCrosslinker(const Crosslinker::Type type, const Crosslinker::Terminus terminusToConnect, const Extremity::MicrotubuleType microtubuleToConnectTo, const int32_t position)
    }*/

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
        *iteratorMaxRemainder -= nFreeCrosslinkers;
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
                throw GeneralException("The iterator in Initialiser::initialiseCrosslinkersRandom() returned a nonexistent position");
                break;
        }
    }
}
