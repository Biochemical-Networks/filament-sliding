#ifndef SYSTEMSTATE_HPP
#define SYSTEMSTATE_HPP

#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"
#include "Extremity.hpp"
#include "CrosslinkerContainer.hpp"

#include <cstdint>

#include <vector>


class SystemState
{
private:
    Microtubule m_fixedMicrotubule;
    MobileMicrotubule m_mobileMicrotubule;

    const int32_t m_nPassiveCrosslinkers;
    const int32_t m_nDualCrosslinkers;
    const int32_t m_nActiveCrosslinkers;
    const int32_t m_nCrosslinkers;

    int32_t m_nFreePassiveCrosslinkers;
    int32_t m_nFreeDualCrosslinkers;
    int32_t m_nFreeActiveCrosslinkers;

    CrosslinkerContainer m_passiveCrosslinkers;
    CrosslinkerContainer m_dualCrosslinkers;
    CrosslinkerContainer m_activeCrosslinkers;

/*    // Store the free crosslinkers in the beginning, and store the connected ones in the end of the vectors.
    // This way, m_nFree*Crosslinkers-1 labels the position of the next free crosslinker (* meant here as regular expression)
    std::vector<Crosslinker> m_passiveCrosslinkers;
    std::vector<Crosslinker> m_dualCrosslinkers;
    std::vector<Crosslinker> m_activeCrosslinkers;*/

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

    void fullyConnectFreeCrosslinker(const Crosslinker::Type type,
                                     const Crosslinker::Terminus terminusToConnectToFixedMicrotubule,
                                     const int32_t positionOnFixedMicrotubule,
                                     const int32_t positionOnMobileMicrotubule);

    Crosslinker& connectFreeCrosslinker(const Crosslinker::Type type,
                                const Crosslinker::Terminus terminusToConnect,
                                const Extremity::MicrotubuleType microtubuleToConnectTo,
                                const int32_t position);

    void disconnectPartiallyConnectedCrosslinker(Crosslinker& crosslinker);

    void connectPartiallyConnectedCrosslinker(Crosslinker& crosslinker, const Extremity::MicrotubuleType oppositeMicrotubule, const int32_t positionOnOppositeMicrotubule);

    void update(const double changeMicrotubulePosition);

    double getMicrotubulePosition() const;

    int32_t getNFreeCrosslinkersOfType(const Crosslinker::Type type) const;
    int32_t getNFreeCrosslinkers() const;

    double beginningOverlap() const;
    double endOverlap() const;
    double overlapLength() const;

    int32_t firstSiteOverlapFixed() const;
    int32_t lastSiteOverlapFixed() const;
    int32_t firstSiteOverlapMobile() const;
    int32_t lastSiteOverlapMobile() const;

    int32_t getNSitesOverlapFixed() const;
    int32_t getNSitesOverlapMobile() const;

    int32_t getNFreeSites() const;
    int32_t getNFreeSitesFixed() const;
    int32_t getNFreeSitesMobile() const;

    int32_t getFreeSitePosition(const Extremity::MicrotubuleType microtubuleType, const int32_t whichFreeSite) const;
};

#endif // SYSTEMSTATE_HPP
