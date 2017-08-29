#include "SystemState.hpp"
#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"
#include "Extremity.hpp"
#include "GeneralException/GeneralException.hpp"

#include <stdexcept>
#include <algorithm>
#include <cmath>

#include <iostream>

SystemState::SystemState(const double lengthMobileMicrotubule,
                            const double lengthFixedMicrotubule,
                            const double latticeSpacing,
                            const int32_t nActiveCrosslinkers,
                            const int32_t nDualCrosslinkers,
                            const int32_t nPassiveCrosslinkers)
    :   m_fixedMicrotubule(lengthFixedMicrotubule, latticeSpacing),
        m_mobileMicrotubule(lengthMobileMicrotubule, latticeSpacing),
        m_nPassiveCrosslinkers(nPassiveCrosslinkers),
        m_nDualCrosslinkers(nDualCrosslinkers),
        m_nActiveCrosslinkers(nActiveCrosslinkers),
        m_nCrosslinkers(m_nActiveCrosslinkers+m_nDualCrosslinkers+m_nPassiveCrosslinkers),
        m_nFreePassiveCrosslinkers(m_nPassiveCrosslinkers),
        m_nFreeDualCrosslinkers(m_nDualCrosslinkers),
        m_nFreeActiveCrosslinkers(m_nActiveCrosslinkers),
        m_passiveCrosslinkers(m_nPassiveCrosslinkers, Crosslinker(Crosslinker::Type::PASSIVE, false, false)),
        m_dualCrosslinkers(m_nDualCrosslinkers, Crosslinker(Crosslinker::Type::DUAL, false, false)),
        m_activeCrosslinkers(m_nActiveCrosslinkers, Crosslinker(Crosslinker::Type::ACTIVE, false, false))
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
Crosslinker& SystemState::connectFreeCrosslinker(const Crosslinker::Type type, const Crosslinker::Terminus terminusToConnect, const Extremity::MicrotubuleType microtubuleToConnectTo, const int32_t position)
{
    std::vector<Crosslinker> *p_crosslinkersVector = nullptr;
    // Get the number of free crosslinkers of a certain type, because this number is used to label the position of the next crosslinker which needs to be connected in its respective vector
    int32_t *p_nFreeCrosslinkers;

    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            p_crosslinkersVector = &m_passiveCrosslinkers;
            p_nFreeCrosslinkers = &m_nFreePassiveCrosslinkers;
            break;
        case Crosslinker::Type::DUAL:
            p_crosslinkersVector = &m_dualCrosslinkers;
            p_nFreeCrosslinkers = &m_nFreeDualCrosslinkers;
            break;
        case Crosslinker::Type::ACTIVE:
            p_crosslinkersVector = &m_activeCrosslinkers;
            p_nFreeCrosslinkers = &m_nFreeActiveCrosslinkers;
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to connectCrosslinker");
            break;
    }

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


    try
    {
        // Everything needs to be defined within the try block, otherwise crosslinkerToConnect is not defined in the right scope
        Crosslinker &crosslinkerToConnect = p_crosslinkersVector->at((*p_nFreeCrosslinkers)-1); // Reference, because only one crosslinker is connected in this function

        crosslinkerToConnect.connectFromFree(microtubuleToConnectTo, terminusToConnect, position); // Connect the crosslinker

        p_microtubuleToConnect->connectSite(position, crosslinkerToConnect, terminusToConnect);

        --(*p_nFreeCrosslinkers); // The number of free crosslinkers of this type decreases upon being connected

        return crosslinkerToConnect; // Such that the caller can use this specific crosslinker immediately
    }
    catch(std::out_of_range) // For the at function
    {
        throw GeneralException("There are no free crosslinkers, but still one was tried to be connected");
    }

}

void SystemState::disconnectPartiallyConnectedCrosslinker(Crosslinker& crosslinker)
{
    Extremity::MicrotubuleType microtubuleToDisconnectFrom;
    int32_t positionToDisconnectFrom;
    crosslinker.getBindingPositionWhenPartiallyConnected(microtubuleToDisconnectFrom, positionToDisconnectFrom); // Both are passed by reference, to get two return values

    Crosslinker::Type type = crosslinker.getType();

    crosslinker.disconnectFromPartialConnection();

    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            ++m_nFreePassiveCrosslinkers;
            break;
        case Crosslinker::Type::DUAL:
            ++m_nFreeDualCrosslinkers;
            break;
        case Crosslinker::Type::ACTIVE:
            ++m_nFreeActiveCrosslinkers;
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to disconnectPartiallyConnectedCrosslinker()");
            break;
    }

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

void SystemState::connectPartiallyConnectedCrosslinker(Crosslinker& crosslinker, const Extremity::MicrotubuleType oppositeMicrotubule, const int32_t positionOnOppositeMicrotubule)
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

    Crosslinker::Terminus terminusToConnect = crosslinker.getFreeTerminusWhenPartiallyConnected();

    crosslinker.fullyConnectFromPartialConnection(oppositeMicrotubule, positionOnOppositeMicrotubule);

    p_microtubuleToConnect->connectSite(positionOnOppositeMicrotubule, crosslinker, terminusToConnect);
}


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
            return m_nFreePassiveCrosslinkers;
            break;
        case Crosslinker::Type::DUAL:
            return m_nFreeDualCrosslinkers;
            break;
        case Crosslinker::Type::ACTIVE:
            return m_nFreeActiveCrosslinkers;
            break;
        default:
            throw GeneralException("An incorrect crosslinker type was passed to getNFreeCrosslinkersOfType()");
            break;
    }
}

int32_t SystemState::getNFreeCrosslinkers() const
{
    return m_nFreePassiveCrosslinkers + m_nFreeDualCrosslinkers + m_nFreeActiveCrosslinkers;
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
        return static_cast <int32_t> (std::floor(pos / m_fixedMicrotubule.getLatticeSpacing()));
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
        return static_cast <int32_t> (std::ceil(pos / m_fixedMicrotubule.getLatticeSpacing()));
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
        return static_cast <int32_t> (std::floor(pos / m_mobileMicrotubule.getLatticeSpacing()));
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
        return static_cast <int32_t> (std::ceil(pos / m_mobileMicrotubule.getLatticeSpacing()));
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



