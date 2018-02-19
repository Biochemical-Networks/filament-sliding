#include "SystemState.hpp"
#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"
#include "CrosslinkerContainer.hpp"
#include "MicrotubuleType.hpp"
#include "PossibleFullConnection.hpp"

#include <algorithm> // max/min
#include <cmath> // ceil/floor/abs
#include <deque>


SystemState::SystemState(const double lengthMobileMicrotubule,
                            const double lengthFixedMicrotubule,
                            const double latticeSpacing,
                            const int32_t nActiveCrosslinkers,
                            const int32_t nDualCrosslinkers,
                            const int32_t nPassiveCrosslinkers)
    :   m_maxStretch(m_maxStretchPerLatticeSpacing*latticeSpacing),
        m_latticeSpacing(latticeSpacing),
        m_fixedMicrotubule(MicrotubuleType::FIXED, lengthFixedMicrotubule, latticeSpacing),
        m_mobileMicrotubule(lengthMobileMicrotubule, latticeSpacing),
        m_nPassiveCrosslinkers(nPassiveCrosslinkers),
        m_nDualCrosslinkers(nDualCrosslinkers),
        m_nActiveCrosslinkers(nActiveCrosslinkers),
        m_nCrosslinkers(m_nActiveCrosslinkers+m_nDualCrosslinkers+m_nPassiveCrosslinkers),
        m_passiveCrosslinkers(m_nPassiveCrosslinkers, Crosslinker(Crosslinker::Type::PASSIVE),Crosslinker::Type::PASSIVE),
        m_dualCrosslinkers(m_nDualCrosslinkers, Crosslinker(Crosslinker::Type::DUAL),Crosslinker::Type::DUAL),
        m_activeCrosslinkers(m_nActiveCrosslinkers, Crosslinker(Crosslinker::Type::ACTIVE),Crosslinker::Type::ACTIVE)
{
}

SystemState::~SystemState()
{
}

void SystemState::setMicrotubulePosition(const double initialPosition)
{
    if (m_nCrosslinkers!=getNFreeCrosslinkers())
    {
        throw GeneralException("setMicrotubulePosition() was called on a system which has connected linkers");
    }
    m_mobileMicrotubule.setPosition(initialPosition);
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
            throw GeneralException("An incorrect microtubule type was passed to connectCrosslinker");
            break;
    }

    Crosslinker* p_connectingCrosslinker;

    // Now, the members containing all crosslinkers of each type are called to connect a free crosslinker in their administration, and to return a pointer to the one that it connected.
    // Then, the pointer can be used to connect the crosslinker in its own administration as well.
    // Done this way, such that the containers don't have to know about the position or microtubule that needs to be connected.
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            p_connectingCrosslinker = m_passiveCrosslinkers.connectFromFreeToPartial();
            break;
        case Crosslinker::Type::DUAL:
            p_connectingCrosslinker = m_dualCrosslinkers.connectFromFreeToPartial();
            break;
        case Crosslinker::Type::ACTIVE:
            p_connectingCrosslinker = m_activeCrosslinkers.connectFromFreeToPartial();
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to connectFreeCrosslinker()");
            break;
    }

    // Connect crosslinker in its own administration
    p_connectingCrosslinker->connectFromFree(terminusToConnect, locationToConnectTo); // Connect the crosslinker

    // Then, perform the connection in the administration of the microtubule
    p_microtubuleToConnect->connectSite(locationToConnectTo.position, *p_connectingCrosslinker, terminusToConnect);

    // Finally, update the information on possibilities with the new SystemState
    m_passiveCrosslinkers.updateConnectionDataFreeToPartial(p_connectingCrosslinker, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
    m_dualCrosslinkers.updateConnectionDataFreeToPartial(p_connectingCrosslinker, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
    m_activeCrosslinkers.updateConnectionDataFreeToPartial(p_connectingCrosslinker, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);

    return *p_connectingCrosslinker; // Such that the caller can use this specific crosslinker immediately
}

void SystemState::disconnectPartiallyConnectedCrosslinker(Crosslinker& disconnectingCrosslinker)
{
    SiteLocation locationToDisconnectFrom = disconnectingCrosslinker.getBoundLocationWhenPartiallyConnected();

    Crosslinker::Type type = disconnectingCrosslinker.getType();

    // Disconnect in administration of crosslinker
    disconnectingCrosslinker.disconnectFromPartialConnection();

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
            throw GeneralException("An incorrect crosslinker type was passed to disconnectPartiallyConnectedCrosslinker()");
            break;
    }

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
            throw GeneralException("An incorrect microtubule type was passed to disconnectPartiallyConnectedCrosslinker()");
            break;
    }

    // Finally, update the information on possibilities with the new SystemState
    m_passiveCrosslinkers.updateConnectionDataPartialToFree(&disconnectingCrosslinker, locationToDisconnectFrom, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
    m_dualCrosslinkers.updateConnectionDataPartialToFree(&disconnectingCrosslinker, locationToDisconnectFrom, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
    m_activeCrosslinkers.updateConnectionDataPartialToFree(&disconnectingCrosslinker, locationToDisconnectFrom, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);

}

void SystemState::connectPartiallyConnectedCrosslinker(Crosslinker& connectingCrosslinker, const SiteLocation locationOppositeMicrotubule)
{
    // Test whether the connection is not outside of the range set by m_maxStretch
    SiteLocation locationThisMicrotubule = connectingCrosslinker.getBoundLocationWhenPartiallyConnected();
    double positionOnFixedMicrotubule;
    double positionOnMobileMicrotubule;

    Microtubule *p_microtubuleToConnect = nullptr;
    switch(locationOppositeMicrotubule.microtubule)
    {
        case MicrotubuleType::FIXED:
            p_microtubuleToConnect = &m_fixedMicrotubule;
            positionOnFixedMicrotubule = locationOppositeMicrotubule.position*m_latticeSpacing;
            positionOnMobileMicrotubule = locationThisMicrotubule.position*m_latticeSpacing + m_mobileMicrotubule.getPosition();
            break;
        case MicrotubuleType::MOBILE:
            p_microtubuleToConnect = &m_mobileMicrotubule;
            positionOnFixedMicrotubule = locationThisMicrotubule.position*m_latticeSpacing;
            positionOnMobileMicrotubule = locationOppositeMicrotubule.position*m_latticeSpacing + m_mobileMicrotubule.getPosition();
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to connectCrosslinker()");
            break;
    }

    if(std::abs(positionOnFixedMicrotubule-positionOnMobileMicrotubule)>=m_maxStretch)
    {
        throw GeneralException("A full connection attempt was made creating an overstretched crosslinker");
    }

    Crosslinker::Terminus terminusToConnect = connectingCrosslinker.getFreeTerminusWhenPartiallyConnected();

    // Connect in administration of crosslinker
    connectingCrosslinker.fullyConnectFromPartialConnection(locationOppositeMicrotubule);

    // Connect in administration of microtubule
    p_microtubuleToConnect->connectSite(locationOppositeMicrotubule.position, connectingCrosslinker, terminusToConnect);

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
            throw GeneralException("An incorrect crosslinker type was passed to disconnectPartiallyConnectedCrosslinker()");
            break;
    }

    // Finally, update the information on possibilities with the new SystemState
    m_passiveCrosslinkers.updateConnectionDataPartialToFull(&connectingCrosslinker, locationOppositeMicrotubule, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
    m_dualCrosslinkers.updateConnectionDataPartialToFull(&connectingCrosslinker, locationOppositeMicrotubule, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
    m_activeCrosslinkers.updateConnectionDataPartialToFull(&connectingCrosslinker, locationOppositeMicrotubule, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);

}

void SystemState::disconnectFullyConnectedCrosslinker(Crosslinker& disconnectingCrosslinker, const Crosslinker::Terminus terminusToDisconnect)
{
    // Retrieve the microtubule and position on that microtubule where the crosslinker is connected
    SiteLocation locationToDisconnectFrom = disconnectingCrosslinker.getOneBoundLocationWhenFullyConnected(terminusToDisconnect);

    Crosslinker::Type type = disconnectingCrosslinker.getType();

    // Disconnect in administration of crosslinker
    disconnectingCrosslinker.disconnectFromFullConnection(terminusToDisconnect);

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
            throw GeneralException("An incorrect crosslinker type was passed to disconnectFullyConnectedCrosslinker()");
            break;
    }

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
            throw GeneralException("An incorrect microtubule type was passed to disconnectFullyConnectedCrosslinker()");
            break;
    }

    // Finally, update the information on possibilities with the new SystemState
    m_passiveCrosslinkers.updateConnectionDataFullToPartial(&disconnectingCrosslinker, locationToDisconnectFrom, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
    m_dualCrosslinkers.updateConnectionDataFullToPartial(&disconnectingCrosslinker, locationToDisconnectFrom, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
    m_activeCrosslinkers.updateConnectionDataFullToPartial(&disconnectingCrosslinker, locationToDisconnectFrom, m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);

}

// This function performs all steps to go from a free to a fully connected crosslinker
void SystemState::fullyConnectFreeCrosslinker(const Crosslinker::Type type,
                                              const Crosslinker::Terminus terminusToConnectToFixedMicrotubule,
                                              const int32_t positionOnFixedMicrotubule,
                                              const int32_t positionOnMobileMicrotubule)
{
    // Store a reference to the connected crosslinker, such that the next function can be called easily

    Crosslinker &connectedCrosslinker = connectFreeCrosslinker(type, terminusToConnectToFixedMicrotubule, SiteLocation{MicrotubuleType::FIXED, positionOnFixedMicrotubule});

    connectPartiallyConnectedCrosslinker(connectedCrosslinker, SiteLocation{MicrotubuleType::MOBILE, positionOnMobileMicrotubule});

}

void SystemState::update(const double changeMicrotubulePosition)
{
    // TODO: TEST WHETHER THE CHANGE IS ALLOWED GIVEN THE CURRENT STATE OF THE CROSSLINKERS
    m_mobileMicrotubule.updatePosition(changeMicrotubulePosition);
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
            return m_dualCrosslinkers.getNPartialCrosslinkers();
            break;
        case Crosslinker::Type::ACTIVE:
            return m_activeCrosslinkers.getNFullCrosslinkers();
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to getNFreeCrosslinkersOfType()");
            break;
    }
}

int32_t SystemState::getNFreeCrosslinkers() const
{
    return m_passiveCrosslinkers.getNFreeCrosslinkers() + m_dualCrosslinkers.getNFreeCrosslinkers() + m_activeCrosslinkers.getNFreeCrosslinkers();
}

double SystemState::beginningOverlap() const
{
    // 0.0 is the position of the beginning of the fixed microtubule
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

    if(overlap <=(-m_maxStretch))
    {
        throw GeneralException("The overlap disappeared");
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


int32_t SystemState::getNSitesOverlapFixed() const
{
    int32_t nSites = lastSiteOverlapFixed()-firstSiteOverlapFixed()+1;
    return nSites;
}

int32_t SystemState::getNSitesOverlapMobile() const
{
    int32_t nSites = lastSiteOverlapMobile()-firstSiteOverlapMobile()+1;
    return nSites;
}

int32_t SystemState::getNFreeSites() const
{
    return m_fixedMicrotubule.getNFreeSites() + m_mobileMicrotubule.getNFreeSites();
}

int32_t SystemState::getNFreeSitesFixed() const
{
    return m_fixedMicrotubule.getNFreeSites();
}

int32_t SystemState::getNFreeSitesMobile() const
{
    return m_mobileMicrotubule.getNFreeSites();
}

int32_t SystemState::getFreeSitePosition(const MicrotubuleType microtubuleType, const int32_t whichFreeSite) const
{
    switch(microtubuleType)
    {
        case MicrotubuleType::FIXED:
            return m_fixedMicrotubule.getFreeSitePosition(whichFreeSite);
            break;
        case MicrotubuleType::MOBILE:
            return m_mobileMicrotubule.getFreeSitePosition(whichFreeSite);
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to getFreeSitePosition");
            break;
    }
}

double SystemState::getMaxStretch() const
{
    return m_maxStretch;
}

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
            throw GeneralException("An incorrect crosslinker type was passed to getNSitesToBindPartial()");
            break;
    }

    return containerToCheck->getNSitesToBindPartial(m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
}

void SystemState::findPossibleConnections(const Crosslinker::Type type)
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
            throw GeneralException("An incorrect crosslinker type was passed to getNSitesToBindPartial()");
            break;
    }
    containerToCheck->findPossibleConnections(m_fixedMicrotubule, m_mobileMicrotubule, m_maxStretch, m_latticeSpacing);
}

#ifdef MYDEBUG
void SystemState::TESTunbindAFullCrosslinker(const int32_t which, const Crosslinker::Terminus terminusToDisconnect)
{
    Crosslinker* p_linker = m_passiveCrosslinkers.TESTgetAFullCrosslinker(which);
    disconnectFullyConnectedCrosslinker(*p_linker, terminusToDisconnect);

}
#endif //MYDEBUG

