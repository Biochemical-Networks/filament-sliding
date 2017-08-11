#ifndef SYSTEMSTATE_HPP
#define SYSTEMSTATE_HPP

#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"
#include <cstdint>

class SystemState
{
private:
    Microtubule m_fixedMicrotubule;
    MobileMicrotubule m_mobileMicrotubule;

    int32_t m_nActiveCrosslinkers;
    int32_t m_nDualCrosslinkers;
    int32_t m_nPassiveCrosslinkers;
    int32_t m_nCrosslinkers;
    std::vector<Crosslinker> m_crosslinkers;

public:
    SystemState(const double lengthMobileMicrotubule,
                const double lengthFixedMicrotubule,
                const double latticeSpacing,
                const int32_t nActiveCrosslinkers,
                const int32_t nDualCrosslinkers,
                const int32_t nPassiveCrosslinkers);
    ~SystemState();
};

#endif // SYSTEMSTATE_HPP
