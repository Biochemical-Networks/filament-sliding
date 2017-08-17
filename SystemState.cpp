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
        m_nFreeCrosslinkers(m_nCrosslinkers),
        m_crosslinkerFree(m_nFreeCrosslinkers, true)
{
    /* Create the crosslinkers here. The vector already exists, but the crosslinker constructor needs to be called knowing what type the crosslinker is (since the type is constant).
     * The vector creates these crosslinkers when they are pushed back. No vector constructor exists that creates crosslinkers with different constructors.
     * Also, insert is not an option, since it requires the assignment operator for Crosslinker, which was implicitly deleted due to the const member variable.
     * First, create crosslinkers to have both extremities free.
     */

    m_crosslinkers.reserve(m_nCrosslinkers);
    for (int32_t i=0; i<m_nPassiveCrosslinkers; ++i)
    {
        m_crosslinkers.push_back(Crosslinker(Crosslinker::Type::PASSIVE, false, false));
    }
    for (int32_t i=0; i<m_nDualCrosslinkers; ++i)
    {
        m_crosslinkers.push_back(Crosslinker(Crosslinker::Type::DUAL, false, false));
    }
    for (int32_t i=0; i<m_nActiveCrosslinkers; ++i)
    {
        m_crosslinkers.push_back(Crosslinker(Crosslinker::Type::ACTIVE, false, false));
    }
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
