#include "Initialiser.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "GeneralException/GeneralException.hpp"
#include "MicrotubuleDynamics.hpp"

#include "Crosslinker.hpp"
#include "Extremity.hpp"

#include <string>
#include <cstdint>
#include <numeric> // std::iota
#include <algorithm> // std::shuffle, std::max_element, std::min
#include <cmath> // std::ceil
#include <vector>
#include <iterator> // std::distance
#include <iostream>

Initialiser::Initialiser(const double initialPositionMicrotubule,
                         const MicrotubuleDynamics microtubuleDynamics,
                         const double probabilityPartiallyConnectedTip,
                         const double probabilityFullyConnectedTip,
                         const double probabilityPartiallyConnectedBlocked,
                         const double probabilityFullyConnectedBlocked,
                         const double probabilityPartialBoundOnTipOutsideOverlap,
                         const double probabilityTipUnblocked,
                         const double tipSize,
                         const double latticeSpacing)
    :   m_initialPositionMicrotubule(initialPositionMicrotubule),
        m_microtubuleDynamics(microtubuleDynamics),
        m_probabilityPartiallyConnectedTip(probabilityPartiallyConnectedTip),
        m_probabilityFullyConnectedTip(probabilityFullyConnectedTip),
        m_probabilityPartiallyConnectedBlocked(probabilityPartiallyConnectedBlocked),
        m_probabilityFullyConnectedBlocked(probabilityFullyConnectedBlocked),
        m_probabilityPartialBoundOnTipOutsideOverlap(probabilityPartialBoundOnTipOutsideOverlap),
        m_probabilityTipUnblocked(probabilityTipUnblocked),
        m_tipLength(static_cast<int32_t>(std::floor(tipSize/latticeSpacing))+1)
{
    // Translate the string to an enum object Initialiser::InitialCrosslinkerDistribution
    // switch statements do not work with strings

    /*if (initialCrosslinkerDistributionString=="RANDOM")
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
    }*/
}

Initialiser::~Initialiser()
{
}

void Initialiser::initialise(SystemState& systemState, RandomGenerator& generator)
{
    systemState.setMicrotubulePosition(m_initialPositionMicrotubule);

    systemState.setTipSize(m_tipLength);

    initialiseCrosslinkers(systemState, generator);

    initialiseBlockedSites(systemState, generator);
}

void Initialiser::initialiseCrosslinkers(SystemState& systemState, RandomGenerator& generator)
{
    const int32_t nSitesOverlapFixed = systemState.getNSitesOverlapFixed();
    const int32_t nSitesOverlapMobile = systemState.getNSitesOverlapMobile();

    // nSitesOverlapFixed and nSitesOverlapMobile can be different when one microtubule is completely overlapping with the other, then use the shortest
    const int32_t nSitesOverlap = std::min(nSitesOverlapFixed, nSitesOverlapMobile);

    std::vector<int32_t> positionsToConnect(nSitesOverlap);
    std::iota(positionsToConnect.begin(), positionsToConnect.end(), 0); // Fill vector with 0, 1, ..., n-1, such that you have a list of all positions
    std::shuffle(positionsToConnect.begin(), positionsToConnect.end(), generator.getBareGenerator()); // Shuffle the positions

    const double fractionOverlapSitesConnected = m_probabilityPartiallyConnectedTip+m_probabilityFullyConnectedTip;
    #ifdef MYDEBUG
    if(fractionOverlapSitesConnected>1.0)
    {
        throw GeneralException("Initialiser::initialiseCrosslinkers() encountered wrong probabilities.");
    }
    #endif // MYDEBUG

    // The following takes the ceiling, since we would always like to have at least one crosslinker to connect in case there is a finite m_fractionOverlapSitesConnected
    // is zero when m_fractionOverlapSitesConnected is 0.0
    int32_t nSitesToConnect = static_cast<int32_t> (std::ceil(fractionOverlapSitesConnected * nSitesOverlap));

    if (nSitesToConnect > nSitesOverlap) // Make sure nothing went wrong in the conversion from double to int
    {
        nSitesToConnect = nSitesOverlap;
    }

    int32_t nFreeCrosslinkers = systemState.getNFreeCrosslinkers();

    if (nFreeCrosslinkers < nSitesToConnect)
    {
        std::string numberOfFreeCrosslinkers = std::to_string(nFreeCrosslinkers);
        std::string desiredNumberOfCrosslinkers = std::to_string(nSitesToConnect);
        throw GeneralException("In Initialiser::initialiseCrosslinkers(), the number of free crosslinkers is "
                                +numberOfFreeCrosslinkers
                                +", lower than the initial number of crosslinkers needed, "
                                +desiredNumberOfCrosslinkers
                                +", for achieving a well connected system. ");
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

    const double givenBoundTipDenominator = (m_probabilityPartiallyConnectedTip+m_probabilityFullyConnectedTip);
    const double probabilityPartialGivenBoundTip = (givenBoundTipDenominator==0.0)?0.0:
                                                m_probabilityPartiallyConnectedTip/givenBoundTipDenominator;

    // Crosslinkers will usually be initialised in a highly stretched state;
    // The first site in the overlap of the mobile microtubule can be a distance maxStretch from that of the fixed microtubule.
    // Therefore, the program should be allowed to run at least for a little while for the system to assume an equilibrium distribution.
    // The crosslinkers will never cross each other, since they are ordered in the same way on the fixed and mobile microtubules
    // The tail binds to the microtubule first, the head can then bind to the actin filament
    // First make the crosslinkers connect as if they are connected to tip sites.
    // After, when some sites become blocked, change the SiteType of the linker to Blocked and disconnect a portion
    // (assuming that the probability of binding to the blocked sites is always lower than that of binding to tip sites)
    for (int32_t i = 0; i<nPassiveCrosslinkersToConnect; ++connectedSoFar, ++i)
    {
        if(generator.getProbability()<probabilityPartialGivenBoundTip)
        {
            systemState.connectFreeCrosslinker(Crosslinker::Type::PASSIVE,
                                               Crosslinker::Terminus::TAIL,
                                               SiteLocation{MicrotubuleType::FIXED, firstSiteOverlapFixed+positionsToConnect.at(connectedSoFar), SiteType::TIP});
        }
        else
        {
            systemState.fullyConnectFreeCrosslinker(Crosslinker::Type::PASSIVE,
                                              Crosslinker::Terminus::TAIL,
                                              firstSiteOverlapFixed+positionsToConnect.at(connectedSoFar), // The position on the fixed microtubule
                                              firstSiteOverlapMobile+positionsToConnect.at(connectedSoFar));
        }
    }

    for (int32_t i = 0; i<nDualCrosslinkersToConnect; ++connectedSoFar, ++i)
    {
        if(generator.getProbability()<probabilityPartialGivenBoundTip)
        {
            systemState.connectFreeCrosslinker(Crosslinker::Type::DUAL,
                                               Crosslinker::Terminus::TAIL,
                                               SiteLocation{MicrotubuleType::FIXED, firstSiteOverlapFixed+positionsToConnect.at(connectedSoFar), SiteType::TIP});
        }
        else
        {
            systemState.fullyConnectFreeCrosslinker(Crosslinker::Type::DUAL,
                                              Crosslinker::Terminus::TAIL,
                                              firstSiteOverlapFixed+positionsToConnect.at(connectedSoFar), // The position on the fixed microtubule
                                              firstSiteOverlapMobile+positionsToConnect.at(connectedSoFar));
        }
    }

    for (int32_t i = 0; i<nActiveCrosslinkersToConnect; ++connectedSoFar, ++i)
    {
        if(generator.getProbability()<probabilityPartialGivenBoundTip)
        {
            systemState.connectFreeCrosslinker(Crosslinker::Type::ACTIVE,
                                               Crosslinker::Terminus::TAIL,
                                               SiteLocation{MicrotubuleType::FIXED, firstSiteOverlapFixed+positionsToConnect.at(connectedSoFar), SiteType::TIP});
        }
        else
        {
            systemState.fullyConnectFreeCrosslinker(Crosslinker::Type::ACTIVE,
                                              Crosslinker::Terminus::TAIL,
                                              firstSiteOverlapFixed+positionsToConnect.at(connectedSoFar), // The position on the fixed microtubule
                                              firstSiteOverlapMobile+positionsToConnect.at(connectedSoFar));
        }
    }

    #ifdef MYDEBUG
    if (connectedSoFar != nSitesToConnect)
    {
        throw GeneralException("Something went wrong in Initialiser::initialiseCrosslinkers(), the number of connected crosslinkers failed to equal the number of sites to connect");
    }
    #endif // MYDEBUG

    // Connect partials outside overlap. The ones inside the overlap are already connected

    for(int32_t i=0; i<systemState.getNSites(MicrotubuleType::FIXED); ++i)
    {
        if((i<systemState.firstSiteOverlapFixed() || i>systemState.lastSiteOverlapFixed()) &&
           generator.getProbability()<m_probabilityPartialBoundOnTipOutsideOverlap)
        {
            if(systemState.getNFreeCrosslinkers()==0)
            {
                throw GeneralException("Not enough passive linkers available in Initialiser::initialiseCrosslinkers().");
            }
            systemState.connectFreeCrosslinker(Crosslinker::Type::PASSIVE,
                                               Crosslinker::Terminus::TAIL,
                                               SiteLocation{MicrotubuleType::FIXED, i, SiteType::TIP});
        }
    }
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

void Initialiser::initialiseBlockedSites(SystemState& systemState, RandomGenerator& generator)
{
    #ifdef MYDEBUG
    if(m_probabilityTipUnblocked<0 || m_probabilityTipUnblocked>1)
    {
        throw GeneralException("Initialiser::initialiseBlockedSites() encountered a wrong value for m_probabilityTipUnblocked");
    }
    #endif // MYDEBUG
    if(m_microtubuleDynamics==MicrotubuleDynamics::STOCHASTIC && m_probabilityTipUnblocked==1.0) return;

    // recalculate the overlap beginning and end as used in Initialiser::initialiseCrosslinkers(),
    // which is necessary to know for deciding whether a specific crosslinker needs to be unbound or not (that depends on whether it is in the overlap)
    const int32_t nSitesOverlap = std::min(systemState.getNSitesOverlapFixed(), systemState.getNSitesOverlapMobile());
    const int32_t firstSiteOverlapFixed = systemState.firstSiteOverlapFixed();
    const int32_t lastSiteOverlapFixed = firstSiteOverlapFixed+nSitesOverlap-1;

    const double disconnectProbabilityUponBlockInOverlap = 1-m_probabilityFullyConnectedBlocked/m_probabilityFullyConnectedTip;
    const double disconnectProbabilityUponBlockOutOfOverlap
        = 1 - (m_probabilityPartiallyConnectedBlocked*(1-m_probabilityFullyConnectedTip))/((1-m_probabilityFullyConnectedBlocked)*m_probabilityPartiallyConnectedTip);

    #ifdef MYDEBUG
    if(disconnectProbabilityUponBlockInOverlap<0.0 || disconnectProbabilityUponBlockInOverlap > 1.0)
    {
        throw GeneralException("Initialiser::initialiseBlockedSites() calculated a wrong disconnectProbabilityUponBlockInOverlap.");
    }

    if(disconnectProbabilityUponBlockOutOfOverlap<0.0 || disconnectProbabilityUponBlockOutOfOverlap > 1.0)
    {
        throw GeneralException("Initialiser::initialiseBlockedSites() calculated a wrong disconnectProbabilityUponBlockOutOfOverlap");
    }

    const double disconnectProbabilityUponBlockInOverlap_2 = 1- m_probabilityPartiallyConnectedBlocked/m_probabilityPartiallyConnectedTip;
    if(std::abs(disconnectProbabilityUponBlockInOverlap_2-disconnectProbabilityUponBlockInOverlap)/disconnectProbabilityUponBlockInOverlap > 0.01)
    {
        throw GeneralException("Initialiser::initialiseBlockedSites() calculated two different unbinding probabilities.");
    }

    std::cout << "disconnectProbabilityUponBlockInOverlap: " << disconnectProbabilityUponBlockInOverlap
              << "\ndisconnectProbabilityUponBlockOutOfOverlap: " << disconnectProbabilityUponBlockOutOfOverlap << '\n';
    int32_t nCrosslinkersOnBlockedInitally=0;
    #endif // MYDEBUG

    const int32_t labelFirstUnblocked = systemState.getNSites(MicrotubuleType::FIXED)-m_tipLength;

    #ifdef MYDEBUG
    if(labelFirstUnblocked<0 || labelFirstUnblocked >= systemState.getNSites(MicrotubuleType::FIXED))
    {
        throw GeneralException("Initialiser::initialiseBlockedSites() encountered a tip region that is larger than the fixed microtubule");
    }
    #endif // MYDEBUG

    int32_t fixedLabel = systemState.getNSites(MicrotubuleType::FIXED)-1;
    double localUnblockedProbability=m_probabilityTipUnblocked;
    while(fixedLabel>=0)
    {
        if((m_microtubuleDynamics==MicrotubuleDynamics::STOCHASTIC && generator.getProbability()>=localUnblockedProbability)
        || (m_microtubuleDynamics==MicrotubuleDynamics::DETERMINISTIC && fixedLabel < labelFirstUnblocked))
        {
            // Since the energy difference between being partially connected or fully connected does not depend on the site state (tip or blocked),
            // there is a fixed probability that a linker in the overlap needs to be disconnected.
            // However, the probability that a linker outside of the overlap should be disconnected is different!
            const bool disconnect = (fixedLabel<firstSiteOverlapFixed || fixedLabel > lastSiteOverlapFixed) ?
                                    generator.getBernoulli(disconnectProbabilityUponBlockOutOfOverlap) :
                                    generator.getBernoulli(disconnectProbabilityUponBlockInOverlap);
            #ifdef MYDEBUG
            const bool crosslinkerOnBlocked =
            #endif // MYDEBUG
            systemState.blockSiteOnFixed(fixedLabel, disconnect);

            #ifdef MYDEBUG
            if(crosslinkerOnBlocked) ++nCrosslinkersOnBlockedInitally;
            #endif // MYDEBUG
        }
        --fixedLabel;
        localUnblockedProbability*=m_probabilityTipUnblocked;
    }

    #ifdef MYDEBUG
    std::cout << "The number of crosslinkers that was initially put on the blocked area: " << nCrosslinkersOnBlockedInitally << '\n';
    #endif // MYDEBUG
}

/*Crosslinker::Terminus Initialiser::terminusToConnectToFixedMicrotubule(RandomGenerator &generator)
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
}*/
