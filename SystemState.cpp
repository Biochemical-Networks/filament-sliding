#include "SystemState.hpp"
#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"
#include "Extremity.hpp"
#include "GeneralException/GeneralException.hpp"

#include <stdexcept>
#include <algorithm>

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
void SystemState::connectFreeCrosslinker(const Crosslinker::Type type, const Crosslinker::Terminus terminusToConnect, const Extremity::MicrotubuleType microtubuleToConnectTo, const int32_t position)
{
    std::vector<Crosslinker> *p_crosslinkersVector = nullptr;
    int32_t nFreeCrosslinkers;
    switch(type)
    {
        case Crosslinker::Type::PASSIVE:
            p_crosslinkersVector = &m_passiveCrosslinkers;
            nFreeCrosslinkers = m_nFreePassiveCrosslinkers;
            break;
        case Crosslinker::Type::DUAL:
            p_crosslinkersVector = &m_dualCrosslinkers;
            nFreeCrosslinkers = m_nFreeDualCrosslinkers;
            break;
        case Crosslinker::Type::ACTIVE:
            p_crosslinkersVector = &m_activeCrosslinkers;
            nFreeCrosslinkers = m_nFreeActiveCrosslinkers;
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
        Crosslinker &crosslinkerToConnect = p_crosslinkersVector->at(nFreeCrosslinkers-1); // Reference, because only one crosslinker is connected in this function

        crosslinkerToConnect.connectFromFree(microtubuleToConnectTo, terminusToConnect, position); // Connect the crosslinker

        p_microtubuleToConnect->connectSite(position, crosslinkerToConnect, terminusToConnect);
    }
    catch(std::out_of_range) // For the at function
    {
        throw GeneralException("There are no free crosslinkers, but still one was tried to be connected");
    }

}

void SystemState::update(const double changeMicrotubulePosition)
{
    m_mobileMicrotubule.updatePosition(changeMicrotubulePosition);
}

double SystemState::getMicrotubulePosition() const
{
    return m_mobileMicrotubule.getPosition();
}

int32_t SystemState::getNFreePassiveCrosslinkers() const
{
    return m_nFreePassiveCrosslinkers;
}

int32_t SystemState::getNFreeDualCrosslinkers() const
{
    return m_nFreeDualCrosslinkers;
}

int32_t SystemState::getNFreeActiveCrosslinkers() const
{
    return m_nFreeActiveCrosslinkers;
}

double SystemState::calculateOverlapLength() const
{
    // Returns a negative value if there is no overlap
    // 0.0 is the position of the beginning of the fixed microtubule
    return std::min(m_fixedMicrotubule.getLength(), m_mobileMicrotubule.getLength() + m_mobileMicrotubule.getPosition()) - std::max(0.0, m_mobileMicrotubule.getPosition());
}
