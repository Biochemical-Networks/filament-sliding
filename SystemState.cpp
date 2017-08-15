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
        m_nActiveCrosslinkers(nActiveCrosslinkers),
        m_nDualCrosslinkers(nDualCrosslinkers),
        m_nPassiveCrosslinkers(nPassiveCrosslinkers),
        m_nCrosslinkers(m_nActiveCrosslinkers+m_nDualCrosslinkers+m_nPassiveCrosslinkers),
        m_crosslinkers(m_nCrosslinkers, Crosslinker(false, false)) // Initialise crosslinkers to have both extremities free
{
}

SystemState::~SystemState()
{
}

void SystemState::initiate(const double initialPosition)
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
