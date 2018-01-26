#include "SystemState.hpp"
#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"
#include "Extremity.hpp"
#include "GeneralException/GeneralException.hpp"

#include <algorithm> // max/min
#include <cmath> // ceil/floor

SystemState::SystemState(const double lengthMobileMicrotubule,
                            const double lengthFixedMicrotubule,
                            const double latticeSpacing,
                            const int32_t nActiveCrosslinkers,
                            const int32_t nDualCrosslinkers,
                            const int32_t nPassiveCrosslinkers)
    :   m_maxStretch(m_maxStretchPerLatticeSpacing*latticeSpacing),
        m_fixedMicrotubule(lengthFixedMicrotubule, latticeSpacing),
        m_mobileMicrotubule(lengthMobileMicrotubule, latticeSpacing),
        m_nPassiveCrosslinkers(nPassiveCrosslinkers),
        m_nDualCrosslinkers(nDualCrosslinkers),
        m_nActiveCrosslinkers(nActiveCrosslinkers),
        m_nCrosslinkers(m_nActiveCrosslinkers+m_nDualCrosslinkers+m_nPassiveCrosslinkers),
        m_passiveCrosslinkers(m_nPassiveCrosslinkers, Crosslinker(Crosslinker::Type::PASSIVE)),
        m_dualCrosslinkers(m_nDualCrosslinkers, Crosslinker(Crosslinker::Type::DUAL)),
        m_activeCrosslinkers(m_nActiveCrosslinkers, Crosslinker(Crosslinker::Type::ACTIVE))
{
}

SystemState::~SystemState()
{
}

void SystemState::setMicrotubulePosition(const double initialPosition)
{
    m_mobileMicrotubule.setPosition(initialPosition);
}

// The following function assumes that it is possible to connect the crosslinker, otherwise it will throw
Crosslinker& SystemState::connectFreeCrosslinker(const Crosslinker::Type type,
                                                 const Crosslinker::Terminus terminusToConnect,
                                                 const Extremity::MicrotubuleType microtubuleToConnectTo,
                                                 const int32_t position)
{
    // Find a pointer to the microtubule that the crosslinker needs to be connected to, such that we can use its methods.
    Microtubule *p_microtubuleToConnect = nullptr;
    switch(microtubuleToConnectTo)
    {
        case Extremity::MicrotubuleType::FIXED:
            p_microtubuleToConnect = &m_fixedMicrotubule;
            break;
        case Extremity::MicrotubuleType::MOBILE:
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
    p_connectingCrosslinker->connectFromFree(microtubuleToConnectTo, terminusToConnect, position); // Connect the crosslinker

    // Then, perform the connection in the administration of the microtubule
    p_microtubuleToConnect->connectSite(position, *p_connectingCrosslinker, terminusToConnect);

    return *p_connectingCrosslinker; // Such that the caller can use this specific crosslinker immediately
}

void SystemState::disconnectPartiallyConnectedCrosslinker(Crosslinker& disconnectingCrosslinker)
{
    Extremity::MicrotubuleType microtubuleToDisconnectFrom;
    int32_t positionToDisconnectFrom;
    disconnectingCrosslinker.getBindingPositionWhenPartiallyConnected(microtubuleToDisconnectFrom, positionToDisconnectFrom); // Both are passed by reference, to get two return values

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
    switch(microtubuleToDisconnectFrom)
    {
        case Extremity::MicrotubuleType::FIXED:
            m_fixedMicrotubule.disconnectSite(positionToDisconnectFrom);
            break;
        case Extremity::MicrotubuleType::MOBILE:
            m_mobileMicrotubule.disconnectSite(positionToDisconnectFrom);
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to disconnectPartiallyConnectedCrosslinker()");
            break;
    }

}

void SystemState::connectPartiallyConnectedCrosslinker(Crosslinker& connectingCrosslinker, const Extremity::MicrotubuleType oppositeMicrotubule, const int32_t positionOnOppositeMicrotubule)
{
    Microtubule *p_microtubuleToConnect = nullptr;
    switch(oppositeMicrotubule)
    {
        case Extremity::MicrotubuleType::FIXED:
            p_microtubuleToConnect = &m_fixedMicrotubule;
            break;
        case Extremity::MicrotubuleType::MOBILE:
            p_microtubuleToConnect = &m_mobileMicrotubule;
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to connectCrosslinker()");
            break;
    }

    Crosslinker::Terminus terminusToConnect = connectingCrosslinker.getFreeTerminusWhenPartiallyConnected();

    // Connect in administration of crosslinker
    connectingCrosslinker.fullyConnectFromPartialConnection(oppositeMicrotubule, positionOnOppositeMicrotubule);

    // Connect in administration of microtubule
    p_microtubuleToConnect->connectSite(positionOnOppositeMicrotubule, connectingCrosslinker, terminusToConnect);

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
}

void SystemState::disconnectFullyConnectedCrosslinker(Crosslinker& disconnectingCrosslinker, const Crosslinker::Terminus terminusToDisconnect)
{
    // Retrieve the microtubule and position on that microtubule where the crosslinker is connected
    Extremity::MicrotubuleType microtubuleToDisconnectFrom;
    int32_t positionToDisconnectFrom;
    disconnectingCrosslinker.getOneBindingPositionWhenFullyConnected(terminusToDisconnect, microtubuleToDisconnectFrom, positionToDisconnectFrom);

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
    switch(microtubuleToDisconnectFrom)
    {
        case Extremity::MicrotubuleType::FIXED:
            m_fixedMicrotubule.disconnectSite(positionToDisconnectFrom);
            break;
        case Extremity::MicrotubuleType::MOBILE:
            m_mobileMicrotubule.disconnectSite(positionToDisconnectFrom);
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to disconnectFullyConnectedCrosslinker()");
            break;
    }

}

// This function performs all steps to go from a free to a fully connected crosslinker
void SystemState::fullyConnectFreeCrosslinker(const Crosslinker::Type type,
                                              const Crosslinker::Terminus terminusToConnectToFixedMicrotubule,
                                              const int32_t positionOnFixedMicrotubule,
                                              const int32_t positionOnMobileMicrotubule)
{
    // Store a reference to the connected crosslinker, such that the next function can be called easily

    Crosslinker &connectedCrosslinker = connectFreeCrosslinker(type, terminusToConnectToFixedMicrotubule, Extremity::MicrotubuleType::FIXED, positionOnFixedMicrotubule);

    connectPartiallyConnectedCrosslinker(connectedCrosslinker, Extremity::MicrotubuleType::MOBILE, positionOnMobileMicrotubule);

}

void SystemState::update(const double changeMicrotubulePosition)
{
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
    return endOverlap()-beginningOverlap();
}

// Return the first and last site of the overlap where crosslinkers could connect. They can stretch one lattice spacing, so a site just next to the strict overlap is still an option.
// Assume that the overlap exists
int32_t SystemState::firstSiteOverlapFixed() const
{
    double pos = beginningOverlap();
    if (pos<0.0) // Shouldn't be possible, but just in case the floating point rounds to slightly below zero
    {
        return 0; // The first site of the fixed microtubule
    }
    else
    {
        return static_cast <int32_t> (std::floor(pos / m_fixedMicrotubule.getLatticeSpacing())); // Floor, because the crosslinker can stretch maximally one lattice spacing
    }
}

int32_t SystemState::lastSiteOverlapFixed() const
{
    double pos = endOverlap();
    if (pos>m_fixedMicrotubule.getLength()) // Shouldn't be possible, but just in case the floating point rounds to slightly above the length
    {
        return m_fixedMicrotubule.getNSites(); // The last site of the fixed microtubule
    }
    else
    {
        return static_cast <int32_t> (std::ceil(pos / m_fixedMicrotubule.getLatticeSpacing())); // Ceil, because the crosslinker can stretch maximally one lattice spacing
    }
}

int32_t SystemState::firstSiteOverlapMobile() const
{
    double pos = beginningOverlap()-m_mobileMicrotubule.getPosition();
    if (pos<0.0) // Shouldn't be possible, but just in case the floating point rounds to slightly below zero
    {
        return 0; // The first site of the mobile microtubule
    }
    else
    {
        return static_cast <int32_t> (std::floor(pos / m_mobileMicrotubule.getLatticeSpacing())); // Floor, because the crosslinker can stretch maximally one lattice spacing
    }
}

int32_t SystemState::lastSiteOverlapMobile() const
{
    double pos = endOverlap()-m_mobileMicrotubule.getPosition();
    if (pos>m_mobileMicrotubule.getLength()) // Shouldn't be possible, but just in case the floating point rounds to slightly above the length
    {
        return m_mobileMicrotubule.getNSites(); // The last site of the mobile microtubule
    }
    else
    {
        return static_cast <int32_t> (std::ceil(pos / m_mobileMicrotubule.getLatticeSpacing())); // Ceil, because the crosslinker can stretch maximally one lattice spacing
    }
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

int32_t SystemState::getFreeSitePosition(const Extremity::MicrotubuleType microtubuleType, const int32_t whichFreeSite) const
{
    switch(microtubuleType)
    {
        case Extremity::MicrotubuleType::FIXED:
            return m_fixedMicrotubule.getFreeSitePosition(whichFreeSite);
            break;
        case Extremity::MicrotubuleType::MOBILE:
            return m_mobileMicrotubule.getFreeSitePosition(whichFreeSite);
            break;
        default:
            throw GeneralException("An incorrect microtubule type was passed to getFreeSitePosition");
            break;
    }
}



