#include "SystemState.hpp"
#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"

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
