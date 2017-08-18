#ifndef SYSTEMSTATE_HPP
#define SYSTEMSTATE_HPP

#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"
#include "Extremity.hpp"
#include <cstdint>
#include <iostream>

class SystemState
{
private:
    Microtubule m_fixedMicrotubule;
    MobileMicrotubule m_mobileMicrotubule;

    int32_t m_nPassiveCrosslinkers;
    int32_t m_nDualCrosslinkers;
    int32_t m_nActiveCrosslinkers;
    int32_t m_nCrosslinkers;

    int32_t m_nFreePassiveCrosslinkers;
    int32_t m_nFreeDualCrosslinkers;
    int32_t m_nFreeActiveCrosslinkers;

    // Store the free crosslinkers in the beginning, and store the connected ones in the end of the vectors.
    // This way, m_nFree*Crosslinkers-1 labels the position of the next free crosslinker
    std::vector<Crosslinker> m_passiveCrosslinkers;
    std::vector<Crosslinker> m_dualCrosslinkers;
    std::vector<Crosslinker> m_activeCrosslinkers;

    //std::vector<bool> m_crosslinkerFree;

    // The force on the microtubule is a property of the system as a whole, because it is caused by the crosslinkers and a possible external force
    double m_forceMicrotubule;
    double m_energy;

public:
    SystemState(const double lengthMobileMicrotubule,
                const double lengthFixedMicrotubule,
                const double latticeSpacing,
                const int32_t nActiveCrosslinkers,
                const int32_t nDualCrosslinkers,
                const int32_t nPassiveCrosslinkers);
    ~SystemState();

    void setMicrotubulePosition(const double positionMicrotubule);

    void connectFreeCrosslinker(const Crosslinker::Type type, const Crosslinker::Terminus terminusToConnect, const Extremity::MicrotubuleType microtubuleToConnectTo, const int32_t position);

    void update(const double changeMicrotubulePosition);

    double getMicrotubulePosition() const;

    int32_t getNFreePassiveCrosslinkers() const;
    int32_t getNFreeDualCrosslinkers() const;
    int32_t getNFreeActiveCrosslinkers() const;
};

#endif // SYSTEMSTATE_HPP
