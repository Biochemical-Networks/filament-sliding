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
#include <utility> // pair
#include <string>

#include <gsl/gsl_sf_psi.h>


SystemState::SystemState(const double lengthMobileMicrotubule,
                            const double lengthFixedMicrotubule,
                            const double latticeSpacing,
                            const double maxStretchPerLatticeSpacing,
                            const int32_t nActiveCrosslinkers,
                            const int32_t nDualCrosslinkers,
                            const int32_t nPassiveCrosslinkers,
                            const double springConstant,
                            const std::string addTheoreticalCounterForce)
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
        m_addTheoreticalCounterForce(addTheoreticalCounterForce=="TRUE") // Whenever it is not true, assume it is false
{
}

SystemState::~SystemState()
{
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
            throw GeneralException("An incorrect microtubule type was passed to SystemState::connectFreeCrosslinker");
            break;
    }

    Crosslinker* p_connectingCrosslinker = nullptr;

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

    const Crosslinker::Terminus disconnectingTerminus = disconnectingCrosslinker.getBoundTerminusWhenPartiallyConnected();

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
    m_passiveCrosslinkers.updateConnectionDataPartialToFree(&disconnectingCrosslinker, locationToDisconnectFrom, disconnectingTerminus);
    m_dualCrosslinkers.updateConnectionDataPartialToFree(&disconnectingCrosslinker, locationToDisconnectFrom, disconnectingTerminus);
    m_activeCrosslinkers.updateConnectionDataPartialToFree(&disconnectingCrosslinker, locationToDisconnectFrom, disconnectingTerminus);

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
    m_passiveCrosslinkers.updateConnectionDataPartialToFull(&connectingCrosslinker, locationOppositeMicrotubule, terminusToConnect);
    m_dualCrosslinkers.updateConnectionDataPartialToFull(&connectingCrosslinker, locationOppositeMicrotubule, terminusToConnect);
    m_activeCrosslinkers.updateConnectionDataPartialToFull(&connectingCrosslinker, locationOppositeMicrotubule, terminusToConnect);

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
    // Store a reference to the connected crosslinker, such that the next function can be called easily

    Crosslinker &connectedCrosslinker = connectFreeCrosslinker(type, terminusToConnectToFixedMicrotubule, SiteLocation{MicrotubuleType::FIXED, positionOnFixedMicrotubule});

    connectPartiallyConnectedCrosslinker(connectedCrosslinker, SiteLocation{MicrotubuleType::MOBILE, positionOnMobileMicrotubule});

}

void SystemState::updateMobilePosition(const double changeMicrotubulePosition)
{
    // This method assumes that the change in the microtubule position is allowed by the fully connected crosslinkers
    m_mobileMicrotubule.updatePosition(changeMicrotubulePosition);

    m_passiveCrosslinkers.updateConnectionDataMobilePositionChange(changeMicrotubulePosition);
    m_dualCrosslinkers.updateConnectionDataMobilePositionChange(changeMicrotubulePosition);
    m_activeCrosslinkers.updateConnectionDataMobilePositionChange(changeMicrotubulePosition);
}

bool SystemState::barrierCrossed()
{
    return m_mobileMicrotubule.barrierCrossed();
}

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

int32_t SystemState::getNPartialCrosslinkersOfType(const Crosslinker::Type type) const
{
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            return m_passiveCrosslinkers.getNPartialCrosslinkers();
            break;
        case Crosslinker::Type::DUAL:
            return m_dualCrosslinkers.getNPartialCrosslinkers();
            break;
        case Crosslinker::Type::ACTIVE:
            return m_activeCrosslinkers.getNPartialCrosslinkers();
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::getNPartialCrosslinkersOfType()");
            break;
    }
}

std::pair<int32_t,int32_t> SystemState::getNPartialCrosslinkersBoundWithHeadAndTailOfType(const Crosslinker::Type type) const
{
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            return m_passiveCrosslinkers.getNPartialsBoundWithHeadAndTail();
            break;
        case Crosslinker::Type::DUAL:
            return m_dualCrosslinkers.getNPartialsBoundWithHeadAndTail();
            break;
        case Crosslinker::Type::ACTIVE:
            return m_activeCrosslinkers.getNPartialsBoundWithHeadAndTail();
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to SystemState::getNPartialCrosslinkersBoundWithHeadAndTailOfType()");
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


int32_t SystemState::getNSitesOverlapFixed() const
{
    return lastSiteOverlapFixed()-firstSiteOverlapFixed()+1;
}

int32_t SystemState::getNSitesOverlapMobile() const
{
    return lastSiteOverlapMobile()-firstSiteOverlapMobile()+1;
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
            throw GeneralException("An incorrect microtubule type was passed to SystemState::getFreeSitePosition()");
            break;
    }
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

const std::vector<PossiblePartialHop>& SystemState::getPossiblePartialHops(const Crosslinker::Type type) const
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
}

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

const std::vector<Crosslinker*>& SystemState::getPartialLinkersBoundWithHead(const Crosslinker::Type type) const
{
    switch(type)
    {
    case Crosslinker::Type::PASSIVE:
        return m_passiveCrosslinkers.getPartialCrosslinkersBoundWithHead();
        break;
    case Crosslinker::Type::DUAL:
        return m_dualCrosslinkers.getPartialCrosslinkersBoundWithHead();
        break;
    case Crosslinker::Type::ACTIVE:
        return m_activeCrosslinkers.getPartialCrosslinkersBoundWithHead();
        break;
    default:
        throw GeneralException("An incorrect type was passed to SystemState::getPartialLinkersBoundWithHead()");
    }
}

const std::vector<Crosslinker*>& SystemState::getPartialLinkersBoundWithTail(const Crosslinker::Type type) const
{
    switch(type)
    {
    case Crosslinker::Type::PASSIVE:
        return m_passiveCrosslinkers.getPartialCrosslinkersBoundWithTail();
        break;
    case Crosslinker::Type::DUAL:
        return m_dualCrosslinkers.getPartialCrosslinkersBoundWithTail();
        break;
    case Crosslinker::Type::ACTIVE:
        return m_activeCrosslinkers.getPartialCrosslinkersBoundWithTail();
        break;
    default:
        throw GeneralException("An incorrect type was passed to SystemState::getPartialLinkersBoundWithTail()");
    }
}

void SystemState::updateForceAndEnergy()
{
    const std::vector<FullConnection>& passiveFullConnections = getFullConnections(Crosslinker::Type::PASSIVE);
    const std::vector<FullConnection>& dualFullConnections = getFullConnections(Crosslinker::Type::DUAL);
    const std::vector<FullConnection>& activeFullConnections = getFullConnections(Crosslinker::Type::ACTIVE);

    double totalExtension = 0;
    double totalSquaredExtension = 0;

    for(const FullConnection& fullConnection : passiveFullConnections)
    {
        totalExtension += fullConnection.extension;
        totalSquaredExtension += fullConnection.extension*fullConnection.extension;
    }
    for(const FullConnection& fullConnection : dualFullConnections)
    {
        totalExtension += fullConnection.extension;
        totalSquaredExtension += fullConnection.extension*fullConnection.extension;
    }
    for(const FullConnection& fullConnection : activeFullConnections)
    {
        totalExtension += fullConnection.extension;
        totalSquaredExtension += fullConnection.extension*fullConnection.extension;
    }

    // Force has a minus sign: a positively expanded linker pulls the mobile microtubule to negative values
    m_forceMicrotubule = -m_springConstant*totalExtension;
    m_energy = 0.5*m_springConstant*totalSquaredExtension;
}

double SystemState::getForce() const
{
    // call the updateForceAndEnergy function before!
    return m_forceMicrotubule;
}

double SystemState::getEnergy() const
{
    // call the updateForceAndEnergy function before!
    return m_energy;
}


#ifdef MYDEBUG
void SystemState::TESTunbindAFullCrosslinker(const int32_t which, const Crosslinker::Terminus terminusToDisconnect)
{
    Crosslinker* p_linker = m_passiveCrosslinkers.TESTgetAFullCrosslinker(which);
    disconnectFullyConnectedCrosslinker(*p_linker, terminusToDisconnect);

}
#endif //MYDEBUG

