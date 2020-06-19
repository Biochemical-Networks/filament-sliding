#include "SystemState.hpp"
#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"
#include "CrosslinkerContainer.hpp"
#include "MicrotubuleType.hpp"
#include "PossibleFullConnection.hpp"
#include "MathematicalFunctions.hpp"

#include <algorithm> // max/min/find
#include <cmath> // ceil/floor/abs
#include <deque>
#include <utility> // pair
#include <string>

SystemState::SystemState(const double lengthMobileMicrotubule,
                            const double lengthFixedMicrotubule,
                            const double latticeSpacing,
                            const double maxStretchPerLatticeSpacing,
                            const int32_t nActiveCrosslinkers,
                            const int32_t nDualCrosslinkers,
                            const int32_t nPassiveCrosslinkers,
                            const double springConstant,
                            const bool addExternalForce,
                            const std::string externalForceTypeString,
                            const MicrotubuleDynamics microtubuleDynamics)
    :   m_maxStretchPerLatticeSpacing(maxStretchPerLatticeSpacing),
        m_maxNumberOfCloseSites(static_cast<int32_t> (std::ceil(2*m_maxStretchPerLatticeSpacing))),
        m_maxStretch(m_maxStretchPerLatticeSpacing*latticeSpacing),
        m_latticeSpacing(latticeSpacing),
        m_springConstant(springConstant),
        m_fixedMicrotubule(MicrotubuleType::FIXED, lengthFixedMicrotubule, latticeSpacing),
        m_mobileMicrotubule(lengthMobileMicrotubule, latticeSpacing),
        m_nPassiveCrosslinkers(nPassiveCrosslinkers),
        m_nDualCrosslinkers(nDualCrosslinkers),
        m_nActiveCrosslinkers(nActiveCrosslinkers),
        m_nCrosslinkers(m_nActiveCrosslinkers+m_nDualCrosslinkers+m_nPassiveCrosslinkers),
        m_passiveCrosslinkers(m_nPassiveCrosslinkers, Crosslinker(Crosslinker::Type::PASSIVE),Crosslinker::Type::PASSIVE, m_fixedMicrotubule, m_mobileMicrotubule, m_latticeSpacing, m_maxStretch),
        m_dualCrosslinkers(m_nDualCrosslinkers, Crosslinker(Crosslinker::Type::DUAL),Crosslinker::Type::DUAL, m_fixedMicrotubule, m_mobileMicrotubule, m_latticeSpacing, m_maxStretch),
        m_activeCrosslinkers(m_nActiveCrosslinkers, Crosslinker(Crosslinker::Type::ACTIVE),Crosslinker::Type::ACTIVE, m_fixedMicrotubule, m_mobileMicrotubule, m_latticeSpacing, m_maxStretch),
        m_addExternalForce(addExternalForce),
        m_microtubuleDynamics(microtubuleDynamics)
{
    if(externalForceTypeString=="BARRIERFREE")
    {
        m_externalForceType=ExternalForceType::BARRIERFREE;
    }
    else if(externalForceTypeString=="QUADRATIC")
    {
        m_externalForceType=ExternalForceType::QUADRATIC;
    }
    else
    {
        throw GeneralException("In the SystemState constructor, the given externalForceTypeString does not hold a recognised value.");
    }
}

SystemState::~SystemState()
{
    checkConsistency();
}

void SystemState::setMicrotubulePosition(const double initialPosition)
{
    #ifdef MYDEBUG
    if (m_nCrosslinkers!=getNFreeCrosslinkers())
    {
        throw GeneralException("SystemState::setMicrotubulePosition() was called on a system which has connected linkers");
    }
    #endif // MYDEBUG

    m_mobileMicrotubule.setPosition(initialPosition);

    // since no linkers are connected, the only thing the following function calls do is set the borders of the region within the possibilities are valid
    m_passiveCrosslinkers.resetPossibilities();
    m_dualCrosslinkers.resetPossibilities();
    m_activeCrosslinkers.resetPossibilities();
}

// The following function assumes that it is possible to connect the crosslinker, otherwise it will throw
Crosslinker& SystemState::connectFreeCrosslinker(const Crosslinker::Type type,
                                                 const Crosslinker::Terminus terminusToConnect,
                                                 const SiteLocation locationToConnectTo)
{
    // Find a pointer to the microtubule that the crosslinker needs to be connected to, such that we can use its methods.
    Microtubule *p_microtubuleToConnect = nullptr;
    switch(locationToConnectTo.microtubule)
    {
        case MicrotubuleType::FIXED:
            p_microtubuleToConnect = &m_fixedMicrotubule;
            break;
        case MicrotubuleType::MOBILE:
            p_microtubuleToConnect = &m_mobileMicrotubule;
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to SystemState::connectFreeCrosslinker()");
            break;
    }

    Crosslinker* p_connectingCrosslinker = nullptr;

    // Now, the members containing all crosslinkers of each type are called to connect a free crosslinker in their administration, and to return a pointer to the one that it connected.
    // Then, the pointer can be used to connect the crosslinker in its own administration as well.
    // Done this way, such that the containers don't have to know about the position or microtubule that needs to be connected.
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            p_connectingCrosslinker = m_passiveCrosslinkers.connectFromFreeToPartial(locationToConnectTo.siteType);
            break;
        case Crosslinker::Type::DUAL:
            p_connectingCrosslinker = m_dualCrosslinkers.connectFromFreeToPartial(locationToConnectTo.siteType);
            break;
        case Crosslinker::Type::ACTIVE:
            p_connectingCrosslinker = m_activeCrosslinkers.connectFromFreeToPartial(locationToConnectTo.siteType);
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::connectFreeCrosslinker()");
            break;
    }

    // Connect crosslinker in its own administration
    p_connectingCrosslinker->connectFromFree(terminusToConnect, locationToConnectTo); // Connect the crosslinker

    // Then, perform the connection in the administration of the microtubule
    p_microtubuleToConnect->connectSite(locationToConnectTo.position, *p_connectingCrosslinker, terminusToConnect);

    // Finally, update the information on possibilities with the new SystemState
    m_passiveCrosslinkers.updateConnectionDataFreeToPartial(p_connectingCrosslinker);
    m_dualCrosslinkers.updateConnectionDataFreeToPartial(p_connectingCrosslinker);
    m_activeCrosslinkers.updateConnectionDataFreeToPartial(p_connectingCrosslinker);

    return *p_connectingCrosslinker; // Such that the caller can use this specific crosslinker immediately
}

void SystemState::disconnectPartiallyConnectedCrosslinker(Crosslinker& disconnectingCrosslinker)
{
    #ifdef MYDEBUG
    if(!disconnectingCrosslinker.isPartial())
    {
        throw GeneralException("SystemState::disconnectPartiallyConnectedCrosslinker() was called on a non-partial linker");
    }
    #endif // MYDEBUG

    SiteLocation locationToDisconnectFrom = disconnectingCrosslinker.getBoundLocationWhenPartiallyConnected();

    Crosslinker::Type type = disconnectingCrosslinker.getType();

    // Disconnect in administration of crosslinker container
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            m_passiveCrosslinkers.disconnectFromPartialToFree(disconnectingCrosslinker);
            break;
        case Crosslinker::Type::DUAL:
            m_dualCrosslinkers.disconnectFromPartialToFree(disconnectingCrosslinker);
            break;
        case Crosslinker::Type::ACTIVE:
            m_activeCrosslinkers.disconnectFromPartialToFree(disconnectingCrosslinker);
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::disconnectPartiallyConnectedCrosslinker()");
            break;
    }

    // Disconnect in administration of crosslinker
    disconnectingCrosslinker.disconnectFromPartialConnection();

    // Disconnect in administration of microtubule
    switch(locationToDisconnectFrom.microtubule)
    {
        case MicrotubuleType::FIXED:
            m_fixedMicrotubule.disconnectSite(locationToDisconnectFrom.position);
            break;
        case MicrotubuleType::MOBILE:
            m_mobileMicrotubule.disconnectSite(locationToDisconnectFrom.position);
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to SystemState::disconnectPartiallyConnectedCrosslinker()");
            break;
    }


    // Finally, update the information on possibilities with the new SystemState
    m_passiveCrosslinkers.updateConnectionDataPartialToFree(&disconnectingCrosslinker, locationToDisconnectFrom);
    m_dualCrosslinkers.updateConnectionDataPartialToFree(&disconnectingCrosslinker, locationToDisconnectFrom);
    m_activeCrosslinkers.updateConnectionDataPartialToFree(&disconnectingCrosslinker, locationToDisconnectFrom);

}

void SystemState::connectPartiallyConnectedCrosslinker(Crosslinker& connectingCrosslinker, const SiteLocation locationOppositeMicrotubule)
{
    #ifdef MYDEBUG
    if(!connectingCrosslinker.isPartial())
    {
        throw GeneralException("SystemState::connectPartiallyConnectedCrosslinker() was called on a non-partial linker");
    }

    SiteLocation locationThisMicrotubule = connectingCrosslinker.getBoundLocationWhenPartiallyConnected();

    if(locationThisMicrotubule.microtubule == locationOppositeMicrotubule.microtubule)
    {
        throw GeneralException("SystemState::connectPartiallyConnectedCrosslinker() was asked to connect a crosslinker twice to one microtubule");
    }

    // Test whether the connection is not outside of the range set by m_maxStretch
    // position is calculated relative to the fixed microtubule, because we want to calculate the total stretch for the check
    double positionOnFixedMicrotubule;
    double positionOnMobileMicrotubule;

    switch(locationOppositeMicrotubule.microtubule)
    {
        case MicrotubuleType::FIXED:
            positionOnFixedMicrotubule = locationOppositeMicrotubule.position*m_latticeSpacing;
            positionOnMobileMicrotubule = locationThisMicrotubule.position*m_latticeSpacing + m_mobileMicrotubule.getPosition();
            break;
        case MicrotubuleType::MOBILE:
            positionOnFixedMicrotubule = locationThisMicrotubule.position*m_latticeSpacing;
            positionOnMobileMicrotubule = locationOppositeMicrotubule.position*m_latticeSpacing + m_mobileMicrotubule.getPosition();
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to SystemState::connectPartiallyConnectedCrosslinker()");
            break;
    }

    if(std::abs(positionOnFixedMicrotubule-positionOnMobileMicrotubule)>=m_maxStretch)
    {
        throw GeneralException("A full connection attempt was made creating an overstretched crosslinker in SystemState::connectPartiallyConnectedCrosslinker()");
    }
    #endif // MYDEBUG

    Microtubule *p_microtubuleToConnect = nullptr;
    switch(locationOppositeMicrotubule.microtubule)
    {
        case MicrotubuleType::FIXED:
            p_microtubuleToConnect = &m_fixedMicrotubule;
            break;
        case MicrotubuleType::MOBILE:
            p_microtubuleToConnect = &m_mobileMicrotubule;
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to SystemState::connectPartiallyConnectedCrosslinker()");
            break;
    }

    Crosslinker::Terminus terminusToConnect = connectingCrosslinker.getFreeTerminusWhenPartiallyConnected();

    // Connect in administration of crosslinker container
    switch(connectingCrosslinker.getType())
    {
        case Crosslinker::Type::PASSIVE:
            m_passiveCrosslinkers.connectFromPartialToFull(connectingCrosslinker);
            break;
        case Crosslinker::Type::DUAL:
            m_dualCrosslinkers.connectFromPartialToFull(connectingCrosslinker);
            break;
        case Crosslinker::Type::ACTIVE:
            m_activeCrosslinkers.connectFromPartialToFull(connectingCrosslinker);
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::connectPartiallyConnectedCrosslinker()");
            break;
    }

    // Connect in administration of crosslinker
    connectingCrosslinker.fullyConnectFromPartialConnection(locationOppositeMicrotubule);

    // Connect in administration of microtubule
    p_microtubuleToConnect->connectSite(locationOppositeMicrotubule.position, connectingCrosslinker, terminusToConnect);

    // Finally, update the information on possibilities with the new SystemState
    m_passiveCrosslinkers.updateConnectionDataPartialToFull(&connectingCrosslinker, locationOppositeMicrotubule);
    m_dualCrosslinkers.updateConnectionDataPartialToFull(&connectingCrosslinker, locationOppositeMicrotubule);
    m_activeCrosslinkers.updateConnectionDataPartialToFull(&connectingCrosslinker, locationOppositeMicrotubule);

}

void SystemState::disconnectFullyConnectedCrosslinker(Crosslinker& disconnectingCrosslinker, const Crosslinker::Terminus terminusToDisconnect)
{
    #ifdef MYDEBUG
    if(!disconnectingCrosslinker.isFull())
    {
        throw GeneralException("SystemState::disconnectFullyConnectedCrosslinker() was called on a non-full linker");
    }
    #endif // MYDEBUG

    // Retrieve the microtubule and position on that microtubule where the crosslinker is connected
    SiteLocation locationToDisconnectFrom = disconnectingCrosslinker.getOneBoundLocationWhenFullyConnected(terminusToDisconnect);

    Crosslinker::Type type = disconnectingCrosslinker.getType();

    // Disconnect in administration of crosslinker container
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            m_passiveCrosslinkers.disconnectFromFullToPartial(disconnectingCrosslinker);
            break;
        case Crosslinker::Type::DUAL:
            m_dualCrosslinkers.disconnectFromFullToPartial(disconnectingCrosslinker);
            break;
        case Crosslinker::Type::ACTIVE:
            m_activeCrosslinkers.disconnectFromFullToPartial(disconnectingCrosslinker);
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::disconnectFullyConnectedCrosslinker()");
            break;
    }

    // Disconnect in administration of crosslinker
    disconnectingCrosslinker.disconnectFromFullConnection(terminusToDisconnect);

    // Disconnect in administration of microtubule
    // Microtubules only keep track whether sites are free or occupied, and which crosslinker is connected to where.
    switch(locationToDisconnectFrom.microtubule)
    {
        case MicrotubuleType::FIXED:
            m_fixedMicrotubule.disconnectSite(locationToDisconnectFrom.position);
            break;
        case MicrotubuleType::MOBILE:
            m_mobileMicrotubule.disconnectSite(locationToDisconnectFrom.position);
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to SystemState::disconnectFullyConnectedCrosslinker()");
            break;
    }

    // Finally, update the information on possibilities with the new SystemState
    m_passiveCrosslinkers.updateConnectionDataFullToPartial(&disconnectingCrosslinker, locationToDisconnectFrom);
    m_dualCrosslinkers.updateConnectionDataFullToPartial(&disconnectingCrosslinker, locationToDisconnectFrom);
    m_activeCrosslinkers.updateConnectionDataFullToPartial(&disconnectingCrosslinker, locationToDisconnectFrom);

}

// This function performs all steps to go from a free to a fully connected crosslinker
void SystemState::fullyConnectFreeCrosslinker(const Crosslinker::Type type,
                                              const Crosslinker::Terminus terminusToConnectToFixedMicrotubule,
                                              const int32_t positionOnFixedMicrotubule,
                                              const int32_t positionOnMobileMicrotubule)
{
    const SiteType typeBindingTo = m_fixedMicrotubule.siteIsBlocked(positionOnFixedMicrotubule) ? SiteType::BLOCKED : SiteType::TIP;

    // Store a reference to the connected crosslinker, such that the next function can be called easily
    Crosslinker &connectedCrosslinker = connectFreeCrosslinker(type, terminusToConnectToFixedMicrotubule, SiteLocation{MicrotubuleType::FIXED, positionOnFixedMicrotubule, typeBindingTo});

    // The actin consists only of TIP sites
    connectPartiallyConnectedCrosslinker(connectedCrosslinker, SiteLocation{MicrotubuleType::MOBILE, positionOnMobileMicrotubule, SiteType::TIP});
}

void SystemState::updateMobilePosition(const double changeMicrotubulePosition)
{
    // This method assumes that the change in the microtubule position is allowed by the fully connected crosslinkers
    m_mobileMicrotubule.updatePosition(changeMicrotubulePosition);

    m_passiveCrosslinkers.updateConnectionDataMobilePositionChange(changeMicrotubulePosition);
    m_dualCrosslinkers.updateConnectionDataMobilePositionChange(changeMicrotubulePosition);
    m_activeCrosslinkers.updateConnectionDataMobilePositionChange(changeMicrotubulePosition);
}

void SystemState::growFixed()
{
    // No crosslinkers are affected, only possibilities
    m_fixedMicrotubule.growOneSite();

    m_passiveCrosslinkers.updateConnectionDataMicrotubuleGrowth();
    m_dualCrosslinkers.updateConnectionDataMicrotubuleGrowth();
    m_activeCrosslinkers.updateConnectionDataMicrotubuleGrowth();
}

// Signal whether a crosslinker was left on a blocked site, such that these events can be counted
bool SystemState::blockSiteOnFixed(const int32_t sitePosition, const bool disconnect)
{
    Crosslinker* linker = m_fixedMicrotubule.giveConnectionAt(sitePosition);
    const bool somethingConnected = linker!=nullptr;
    if(somethingConnected && disconnect) // a crosslinker is bound there and we need to disconnect it.
    {
        // Either fully disconnect it or leave it: don't turn a full into a partial here.
        // disconnectIfFull is only set to True if it needs to fully disconnect.
        // Crosslinkers that should be initially partial should be made partial in the first place (when all sites are still tip sites).
        if(linker->isFull())
        {
            disconnectFullyConnectedCrosslinker(*linker, Crosslinker::Terminus::HEAD); // HEAD is always connected to the mobile, unbind that first

            disconnectPartiallyConnectedCrosslinker(*linker);
        }
        else if(linker->isPartial())
        {
            disconnectPartiallyConnectedCrosslinker(*linker);
        }
    }
    else if(somethingConnected && !disconnect)
    {
        // Change the direct administration of CrosslinkerContainer
        switch(linker->getType())
        {
        case Crosslinker::Type::PASSIVE:
            m_passiveCrosslinkers.blockConnectedSite(*linker);
            break;
        case Crosslinker::Type::DUAL:
            m_dualCrosslinkers.blockConnectedSite(*linker);
            break;
        case Crosslinker::Type::ACTIVE:
            m_activeCrosslinkers.blockConnectedSite(*linker);
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was encountered in SystemState::blockSiteOnFixed()");
        }
        // The administration of Crosslinker is changed, since the extremity holds the SiteLocation (including SiteType) it is connected to
        linker->changePosition(SiteLocation{MicrotubuleType::FIXED, sitePosition, SiteType::BLOCKED});
    }

    // Administration of Microtubule:
    m_fixedMicrotubule.blockSite(sitePosition);

    // and finally, update the possibilities, held in the CrosslinkerContainers.
    // Nothing will actually be updated here, since there are no partials on the mobile,
    // and the possible connections do not store the SiteLocation of the partial explicitly.
    m_passiveCrosslinkers.updateConnectionDataBlockSite(sitePosition);
    m_dualCrosslinkers.updateConnectionDataBlockSite(sitePosition);
    m_activeCrosslinkers.updateConnectionDataBlockSite(sitePosition);

    return somethingConnected && (!disconnect);
}

/*int32_t SystemState::barrierCrossed()
{
    return m_mobileMicrotubule.barrierCrossed();
}*/

// Gives the upper and lower bounds to the possible change in mobile microtubule position
std::pair<double, double> SystemState::movementBordersSetByFullLinkers() const
{
    std::pair<double,double> setByPassive = m_passiveCrosslinkers.movementBordersSetByFullLinkers();
    std::pair<double,double> setByDual = m_dualCrosslinkers.movementBordersSetByFullLinkers();
    std::pair<double,double> setByActive = m_activeCrosslinkers.movementBordersSetByFullLinkers();
    // max / min, because the most negative movement allowed by the linkers is given by the first restricter
    return std::pair<double,double>(std::max({setByPassive.first, setByDual.first, setByActive.first}), std::min({setByPassive.second, setByDual.second, setByActive.second}));
}

double SystemState::getMicrotubulePosition() const
{
    return m_mobileMicrotubule.getPosition();
}

int32_t SystemState::getNFreeCrosslinkersOfType(const Crosslinker::Type type) const
{
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            return m_passiveCrosslinkers.getNFreeCrosslinkers();
            break;
        case Crosslinker::Type::DUAL:
            return m_dualCrosslinkers.getNFreeCrosslinkers();
            break;
        case Crosslinker::Type::ACTIVE:
            return m_activeCrosslinkers.getNFreeCrosslinkers();
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::getNFreeCrosslinkersOfType()");
            break;
    }
}

int32_t SystemState::getNPartialCrosslinkersOfType(const Crosslinker::Type linkerType, const SiteType siteType) const
{
    switch(linkerType)
    {
        case Crosslinker::Type::PASSIVE:
            return m_passiveCrosslinkers.getNPartialCrosslinkers(siteType);
            break;
        case Crosslinker::Type::DUAL:
            return m_dualCrosslinkers.getNPartialCrosslinkers(siteType);
            break;
        case Crosslinker::Type::ACTIVE:
            return m_activeCrosslinkers.getNPartialCrosslinkers(siteType);
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::getNPartialCrosslinkersOfType()");
            break;
    }
}

int32_t SystemState::getNFreeCrosslinkers() const
{
    return m_passiveCrosslinkers.getNFreeCrosslinkers()
        + m_dualCrosslinkers.getNFreeCrosslinkers()
        + m_activeCrosslinkers.getNFreeCrosslinkers();
}

int32_t SystemState::getNFullCrosslinkers() const
{
    return m_passiveCrosslinkers.getNFullCrosslinkers()
        + m_dualCrosslinkers.getNFullCrosslinkers()
        + m_activeCrosslinkers.getNFullCrosslinkers();
}

int32_t SystemState::getNFullRightPullingCrosslinkers() const
{
    return m_passiveCrosslinkers.getNFullRightPullingCrosslinkers()
        + m_dualCrosslinkers.getNFullRightPullingCrosslinkers()
        + m_activeCrosslinkers.getNFullRightPullingCrosslinkers();
}

double SystemState::beginningOverlap() const
{
    // 0.0 is the position of the minus end of the fixed microtubule (the origin)
    return std::max(0.0, m_mobileMicrotubule.getPosition());
}

double SystemState::endOverlap() const
{
    return std::min(m_fixedMicrotubule.getLength(), m_mobileMicrotubule.getLength() + m_mobileMicrotubule.getPosition());
}

double SystemState::overlapLength() const
{
    // Returns a negative value if there is no overlap
    double overlap = endOverlap()-beginningOverlap();

    // Leave this test: if the microtubule ever moves into an unbound configuration and drifts away, we should know about it
    if(overlap <=(-m_maxStretch))
    {
        throw GeneralException("The overlap disappeared according to SystemState::overlapLength()");
    }
    return overlap;
}

// Return the first and last site of the overlap where crosslinkers could connect.
// They can stretch m_maxStretchPerLatticeSpacing lattice spacing, so a site just next to the strict overlap is still an option.
// Assume that the overlap exists
int32_t SystemState::firstSiteOverlapFixed() const
{
    double pos = beginningOverlap();
    return m_fixedMicrotubule.getFirstPositionCloseTo(pos,m_maxStretch);
}

int32_t SystemState::lastSiteOverlapFixed() const
{
    double pos = endOverlap();
    return m_fixedMicrotubule.getLastPositionCloseTo(pos,m_maxStretch);
}

int32_t SystemState::firstSiteOverlapMobile() const
{
    double pos = beginningOverlap()-m_mobileMicrotubule.getPosition();
    return m_mobileMicrotubule.getFirstPositionCloseTo(pos,m_maxStretch);
}

int32_t SystemState::lastSiteOverlapMobile() const
{
    double pos = endOverlap()-m_mobileMicrotubule.getPosition();
    return m_mobileMicrotubule.getLastPositionCloseTo(pos,m_maxStretch);
}

bool SystemState::thereIsOverlap() const
{
    return (m_mobileMicrotubule.getPosition() < m_fixedMicrotubule.getLength()+m_maxStretch) && (m_mobileMicrotubule.getPosition()+m_mobileMicrotubule.getLength()+m_maxStretch>0);
}

int32_t SystemState::getNSitesOverlapFixed() const
{
    return thereIsOverlap()?lastSiteOverlapFixed()-firstSiteOverlapFixed()+1:0;
}

int32_t SystemState::getNSitesOverlapMobile() const
{
    return thereIsOverlap()?lastSiteOverlapMobile()-firstSiteOverlapMobile()+1:0;
}

int32_t SystemState::getNFreeSites(const MicrotubuleType microtubuleType, const SiteType siteType) const
{
    switch(microtubuleType)
    {
    case MicrotubuleType::FIXED:
        return m_fixedMicrotubule.getNFreeSites(siteType);
    case MicrotubuleType::MOBILE:
        return m_mobileMicrotubule.getNFreeSites(siteType);
    default:
        throw GeneralException("SystemState::getNFreeSites() was passed a wrong microtubuleType.");
    }
}
int32_t SystemState::getNFreeSites(const SiteType siteType) const
{
    return getNFreeSites(MicrotubuleType::FIXED, siteType) + getNFreeSites(MicrotubuleType::MOBILE, siteType);
}

int32_t SystemState::getNFreeSites(const MicrotubuleType microtubuleType) const
{
    return getNFreeSites(microtubuleType, SiteType::TIP) + getNFreeSites(microtubuleType, SiteType::BLOCKED);
}

int32_t SystemState::getNFreeSites() const
{
    return getNFreeSites(SiteType::TIP) + getNFreeSites(SiteType::BLOCKED);
}

int32_t SystemState::getFreeSitePosition(const MicrotubuleType microtubuleType, const SiteType siteType, const int32_t whichFreeSite) const
{
    switch(microtubuleType)
    {
        case MicrotubuleType::FIXED:
            return m_fixedMicrotubule.getFreeSitePosition(siteType, whichFreeSite);
            break;
        case MicrotubuleType::MOBILE:
            return m_mobileMicrotubule.getFreeSitePosition(siteType, whichFreeSite);
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to SystemState::getFreeSitePosition()");
            break;
    }
}

int32_t SystemState::getUnblockedSitePosition(const BoundState whichBoundState, const int32_t whichUnblockedSite) const
{
    return m_fixedMicrotubule.getUnblockedSitePosition(whichBoundState, whichUnblockedSite);
}

double SystemState::getMaxStretch() const
{
    return m_maxStretch;
}

#ifdef MYDEBUG
int32_t SystemState::getNSitesToBindPartial(const Crosslinker::Type type) const
{
    // Get a pointer to the right container, since each type is stored in a separate container
    const CrosslinkerContainer* containerToCheck = nullptr;
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            containerToCheck = &m_passiveCrosslinkers;
            break;
        case Crosslinker::Type::DUAL:
            containerToCheck = &m_dualCrosslinkers;
            break;
        case Crosslinker::Type::ACTIVE:
            containerToCheck = &m_activeCrosslinkers;
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::getNSitesToBindPartial()");
            break;
    }

    return containerToCheck->getNSitesToBindPartial();
}
#endif // MYDEBUG

void SystemState::findPossibilities(const Crosslinker::Type type)
{
    // Get a pointer to the right container, since each type is stored in a separate container
    CrosslinkerContainer* containerToCheck = nullptr; // Not const, container will be changed
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            containerToCheck = &m_passiveCrosslinkers;
            break;
        case Crosslinker::Type::DUAL:
            containerToCheck = &m_dualCrosslinkers;
            break;
        case Crosslinker::Type::ACTIVE:
            containerToCheck = &m_activeCrosslinkers;
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::findPossibilities()");
            break;
    }
    containerToCheck->resetPossibilities();
}

const std::vector<PossibleFullConnection>& SystemState::getPossibleConnections(const Crosslinker::Type type) const
{
    switch(type)
    {
    case Crosslinker::Type::PASSIVE:
        return m_passiveCrosslinkers.getPossibleConnections();
        break;
    case Crosslinker::Type::DUAL:
        return m_dualCrosslinkers.getPossibleConnections();
        break;
    case Crosslinker::Type::ACTIVE:
        return m_activeCrosslinkers.getPossibleConnections();
        break;
    default:
        throw GeneralException("An incorrect type was passed to SystemState::getPossibleConnections()");
    }
}

/*const std::vector<PossiblePartialHop>& SystemState::getPossiblePartialHops(const Crosslinker::Type type) const
{
    switch(type)
    {
    case Crosslinker::Type::PASSIVE:
        return m_passiveCrosslinkers.getPossiblePartialHops();
        break;
    case Crosslinker::Type::DUAL:
        return m_dualCrosslinkers.getPossiblePartialHops();
        break;
    case Crosslinker::Type::ACTIVE:
        return m_activeCrosslinkers.getPossiblePartialHops();
        break;
    default:
        throw GeneralException("An incorrect type was passed to SystemState::getPossiblePartialHops()");
    }
}

const std::vector<PossibleFullHop>& SystemState::getPossibleFullHops(const Crosslinker::Type type) const
{
    switch(type)
    {
    case Crosslinker::Type::PASSIVE:
        return m_passiveCrosslinkers.getPossibleFullHops();
        break;
    case Crosslinker::Type::DUAL:
        return m_dualCrosslinkers.getPossibleFullHops();
        break;
    case Crosslinker::Type::ACTIVE:
        return m_activeCrosslinkers.getPossibleFullHops();
        break;
    default:
        throw GeneralException("An incorrect type was passed to SystemState::getPossibleFullHops()");
    }
}*/

const std::vector<FullConnection>& SystemState::getFullConnections(const Crosslinker::Type type) const
{
    switch(type)
    {
    case Crosslinker::Type::PASSIVE:
        return m_passiveCrosslinkers.getFullConnections();
        break;
    case Crosslinker::Type::DUAL:
        return m_dualCrosslinkers.getFullConnections();
        break;
    case Crosslinker::Type::ACTIVE:
        return m_activeCrosslinkers.getFullConnections();
        break;
    default:
        throw GeneralException("An incorrect type was passed to SystemState::getFullConnections()");
    }
}

const std::vector<Crosslinker*>& SystemState::getPartialLinkers(const Crosslinker::Type type) const
{
    switch(type)
    {
    case Crosslinker::Type::PASSIVE:
        return m_passiveCrosslinkers.getPartialLinkers();
        break;
    case Crosslinker::Type::DUAL:
        return m_dualCrosslinkers.getPartialLinkers();
        break;
    case Crosslinker::Type::ACTIVE:
        return m_activeCrosslinkers.getPartialLinkers();
        break;
    default:
        throw GeneralException("An incorrect type was passed to SystemState::getPartialLinkers()");
    }
}

const std::vector<Crosslinker*>& SystemState::getPartialLinkers(const Crosslinker::Type type, const SiteType siteType) const
{
    switch(type)
    {
    case Crosslinker::Type::PASSIVE:
        return m_passiveCrosslinkers.getPartialLinkers(siteType);
        break;
    case Crosslinker::Type::DUAL:
        return m_dualCrosslinkers.getPartialLinkers(siteType);
        break;
    case Crosslinker::Type::ACTIVE:
        return m_activeCrosslinkers.getPartialLinkers(siteType);
        break;
    default:
        throw GeneralException("An incorrect type was passed to SystemState::getPartialLinkers()");
    }
}

const std::vector<Crosslinker*>& SystemState::getFullLinkers(const Crosslinker::Type type) const
{
    switch(type)
    {
    case Crosslinker::Type::PASSIVE:
        return m_passiveCrosslinkers.getFullLinkers();
        break;
    case Crosslinker::Type::DUAL:
        return m_dualCrosslinkers.getFullLinkers();
        break;
    case Crosslinker::Type::ACTIVE:
        return m_activeCrosslinkers.getFullLinkers();
        break;
    default:
        throw GeneralException("An incorrect type was passed to SystemState::getFullLinkers()");
    }
}

std::vector<int32_t> SystemState::getBlockedSitePositions(const MicrotubuleType type) const
{
    switch(type)
    {
    case MicrotubuleType::FIXED:
        return m_fixedMicrotubule.getBlockedSitePositions();
    case MicrotubuleType::MOBILE:
        return m_mobileMicrotubule.getBlockedSitePositions();
    default:
        throw GeneralException("SystemState::getBlockedSitePositions() encountered wrong type.");
    }
}

void SystemState::updateForceAndEnergy()
{
    const std::vector<FullConnection>& passiveFullConnections = getFullConnections(Crosslinker::Type::PASSIVE);
    const std::vector<FullConnection>& dualFullConnections = getFullConnections(Crosslinker::Type::DUAL);
    const std::vector<FullConnection>& activeFullConnections = getFullConnections(Crosslinker::Type::ACTIVE);

    double totalExtension = 0;
    /*double totalSquaredExtension = 0;*/

    for(const FullConnection& fullConnection : passiveFullConnections)
    {
        totalExtension += fullConnection.extension;
        /*totalSquaredExtension += fullConnection.extension*fullConnection.extension;*/
    }
    for(const FullConnection& fullConnection : dualFullConnections)
    {
        totalExtension += fullConnection.extension;
        /*totalSquaredExtension += fullConnection.extension*fullConnection.extension;*/
    }
    for(const FullConnection& fullConnection : activeFullConnections)
    {
        totalExtension += fullConnection.extension;
        /*totalSquaredExtension += fullConnection.extension*fullConnection.extension;*/
    }

    // Force has a minus sign: a positively expanded linker pulls the mobile microtubule to negative values
    m_forceMicrotubule = -m_springConstant*totalExtension;
    /*m_energy = 0.5*m_springConstant*totalSquaredExtension;*/
    m_totalExtensionLinkers = totalExtension; // save, for this can be used by the propagator to integrate the force over the time step (instead of assuming a constant force during it)

    // Recalculate the external force every time step, since it can change depending on the position of the microtubule
    m_forceMicrotubule += findExternalForce();
}

double SystemState::getForce() const
{
    // call the updateForceAndEnergy function before!
    return m_forceMicrotubule;
}

/*double SystemState::getEnergy() const
{
    // call the updateForceAndEnergy function before!
    return m_energy;
}*/

double SystemState::getTotalExtensionLinkers() const
{
    // call the updateForceAndEnergy function before!
    return m_totalExtensionLinkers;
}

double SystemState::findExternalForce() const
{
    double externalForce = 0;

    if(m_addExternalForce) // It can be called without it actually doing something
    {
        switch(m_externalForceType)
        {
            case ExternalForceType::BARRIERFREE:
                externalForce = 0; // Not defined for export version
                break;
            case ExternalForceType::QUADRATIC:
                externalForce = 0; // Not yet defined
                break;
            default:
                throw GeneralException("Caller of SystemState::findExternalForce() asked for an unsupported external force");
                break;
        }
    }

    return externalForce;
}

int32_t SystemState::getNSites(const MicrotubuleType microtubule) const
{
    switch(microtubule)
    {
        case MicrotubuleType::FIXED:
            return m_fixedMicrotubule.getNSites();
            break;
        case MicrotubuleType::MOBILE:
            return m_mobileMicrotubule.getNSites();
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to SystemState::getNSites()");
            break;
    }
}

double SystemState::getLatticeSpacing() const
{
    return m_latticeSpacing;
}

int32_t SystemState::getNUnblockedSitesFixed(const BoundState boundState) const
{
    return m_fixedMicrotubule.getNUnblockedSites(boundState);
}

int32_t SystemState::getNUnblockedSitesFixed() const
{
    return getNUnblockedSitesFixed(BoundState::BOUND)+getNUnblockedSitesFixed(BoundState::UNBOUND);
}

double SystemState::getPositionMicrotubuleTip() const
{
    return m_fixedMicrotubule.getLength()- m_fixedMicrotubule.getTipSize();
}

double SystemState::getActinFrontPositionRelativeToTip() const
{
    return m_mobileMicrotubule.getPosition() + m_mobileMicrotubule.getLength() - m_fixedMicrotubule.getLength() + m_fixedMicrotubule.getTipSize();
}

bool SystemState::actinOnTip() const
{
    return m_mobileMicrotubule.getPosition() + m_mobileMicrotubule.getLength() > getPositionMicrotubuleTip();
}

void SystemState::setTipSize(const int32_t tipLength)
{
    m_fixedMicrotubule.setTipSize(tipLength);
}

// Check if all duplicate information in the different parts of the SystemState object is consistent with each other.
// Throw exceptions indicating the inconsistency in case one is found.
void SystemState::checkConsistency()
{
    // Classes containing data:
    // * Extremity
    // * Crosslinker
    // * CrosslinkerContainer
    // * Site
    // * Microtubule

    // Check the internal consistency of the information stored in the microtubules (including the sites)
    m_fixedMicrotubule.checkInternalConsistency();
    m_mobileMicrotubule.checkInternalConsistency();

    // Check the internal consistency of the information stored in the crosslinker containers
    // (including the crosslinkers and with them the extremities, and the possible connections)
    m_passiveCrosslinkers.checkInternalConsistency();
    m_dualCrosslinkers.checkInternalConsistency();
    m_activeCrosslinkers.checkInternalConsistency();

    // Cross-check the information stored in the crosslinker containers against the info in the microtubules
    // First the partial passive linkers on the tip:
    const std::vector<Crosslinker*>& partialPassivesTipCrosslinkerContainer = m_passiveCrosslinkers.getPartialLinkers(SiteType::TIP);
    const std::vector<Crosslinker*> partialPassivesTipFixedMicrotubule = m_fixedMicrotubule.getPartialPassiveLinkers(SiteType::TIP);
    if(partialPassivesTipCrosslinkerContainer.size()!=partialPassivesTipFixedMicrotubule.size())
    {
        throw GeneralException("In SystemState::checkConsistency(): the number of partial passives on the tip is inconsistent");
    }
    for(const Crosslinker*const p_linker : partialPassivesTipCrosslinkerContainer)
    {
        if(std::find(partialPassivesTipFixedMicrotubule.begin(), partialPassivesTipFixedMicrotubule.end(), p_linker)==partialPassivesTipFixedMicrotubule.end())
        {
            throw GeneralException("In SystemState::checkConsistency(): partial passives on the tip are inconsistent");
        }
    }
    // Then the partial passive linkers on the blocked region:
    const std::vector<Crosslinker*>& partialPassivesBlockedCrosslinkerContainer = m_passiveCrosslinkers.getPartialLinkers(SiteType::BLOCKED);
    const std::vector<Crosslinker*> partialPassivesBlockedFixedMicrotubule = m_fixedMicrotubule.getPartialPassiveLinkers(SiteType::BLOCKED);
    if(partialPassivesBlockedCrosslinkerContainer.size()!=partialPassivesBlockedFixedMicrotubule.size())
    {
        throw GeneralException("In SystemState::checkConsistency(): the number of partial passives on the blocked region is inconsistent");
    }
    for(const Crosslinker*const p_linker : partialPassivesBlockedCrosslinkerContainer)
    {
        if(std::find(partialPassivesBlockedFixedMicrotubule.begin(), partialPassivesBlockedFixedMicrotubule.end(), p_linker)==partialPassivesBlockedFixedMicrotubule.end())
        {
            throw GeneralException("In SystemState::checkConsistency(): partial passives on the blocked region are inconsistent");
        }
    }

    // Also the full linkers:
    const std::vector<Crosslinker*> fullPassiveLinkersCrosslinkerContainer = m_passiveCrosslinkers.getFullLinkers();
    const std::vector<Crosslinker*> fullPassiveLinkersFixedMicrotubule = m_fixedMicrotubule.getFullPassiveLinkers();
    if(fullPassiveLinkersCrosslinkerContainer.size()!=fullPassiveLinkersFixedMicrotubule.size())
    {
        throw GeneralException("In SystemState::checkConsistency(): the number of full passives is inconsistent");
    }
    for(const Crosslinker*const p_linker : fullPassiveLinkersCrosslinkerContainer)
    {
        if(std::find(fullPassiveLinkersFixedMicrotubule.begin(), fullPassiveLinkersFixedMicrotubule.end(), p_linker)==fullPassiveLinkersFixedMicrotubule.end())
        {
            throw GeneralException("In SystemState::checkConsistency(): full passives are inconsistent");
        }
    }
}
