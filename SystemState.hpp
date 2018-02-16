#ifndef SYSTEMSTATE_HPP
#define SYSTEMSTATE_HPP

#include "Microtubule.hpp"
#include "MicrotubuleType.hpp"
#include "MobileMicrotubule.hpp"
#include "Crosslinker.hpp"
#include "CrosslinkerContainer.hpp"
#include "PossibleFullConnection.hpp"

#include <cstdint>
#include <cmath>

#include <vector>

// SystemState methods allow the SystemState to be changed; they do not provide the rules by which it is changed

class SystemState
{
private:
    // With a stretch < 1.5 lattice spacing, there are maximally 3 types of stretch at a time (at exactly 1.5, there could be 4).
    // The number is defined smaller than 1.5 to be sure that there are never 4 states possible, in which the state could become locked.
    // 15 decimals seems to be the precision
    constexpr static double m_maxStretchPerLatticeSpacing=1.499999999999999;
    constexpr static int32_t m_maxNumberOfCloseSites = static_cast<int32_t> (std::ceil(2*m_maxStretchPerLatticeSpacing));
    const double m_maxStretch; // To be defined in terms of the lattice spacing and m_maxStretchPerLatticeSpacing
    const double m_latticeSpacing;

    Microtubule m_fixedMicrotubule;
    MobileMicrotubule m_mobileMicrotubule;

    const int32_t m_nPassiveCrosslinkers;
    const int32_t m_nDualCrosslinkers;
    const int32_t m_nActiveCrosslinkers;
    const int32_t m_nCrosslinkers;

    CrosslinkerContainer m_passiveCrosslinkers;
    CrosslinkerContainer m_dualCrosslinkers;
    CrosslinkerContainer m_activeCrosslinkers;

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

    SystemState(const SystemState&) = delete;
    SystemState& operator=(const SystemState&) = delete;

    void setMicrotubulePosition(const double positionMicrotubule);

    void fullyConnectFreeCrosslinker(const Crosslinker::Type type,
                                     const Crosslinker::Terminus terminusToConnectToFixedMicrotubule,
                                     const int32_t positionOnFixedMicrotubule,
                                     const int32_t positionOnMobileMicrotubule);

    Crosslinker& connectFreeCrosslinker(const Crosslinker::Type type,
                                const Crosslinker::Terminus terminusToConnect,
                                const SiteLocation locationToConnectTo);

    void disconnectPartiallyConnectedCrosslinker(Crosslinker& crosslinker);

    void connectPartiallyConnectedCrosslinker(Crosslinker& crosslinker, const SiteLocation locationOppositeMicrotubule);

    void disconnectFullyConnectedCrosslinker(Crosslinker& crosslinker, const Crosslinker::Terminus terminusToDisconnect);

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

    // whichFreeSite gives the location in the storage of the Microtubule objects, does not say anything about which physical site that is.
    int32_t getFreeSitePosition(const MicrotubuleType microtubuleType, const int32_t whichFreeSite) const;

    double getMaxStretch() const;

    int32_t getNSitesToBindPartial(const Crosslinker::Type type) const;

    void findPossibleConnections(const Crosslinker::Type type);

    #ifdef MYDEBUG
    void TESTunbindAFullCrosslinker(const int32_t which, const Crosslinker::Terminus terminusToDisconnect);
    #endif //MYDEBUG
};

#endif // SYSTEMSTATE_HPP
