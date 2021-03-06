#ifndef SYSTEMSTATE_HPP
#define SYSTEMSTATE_HPP

#include <cmath>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/CrosslinkerContainer.hpp"
#include "filament-sliding/FullConnection.hpp"
#include "filament-sliding/Microtubule.hpp"
#include "filament-sliding/MicrotubuleType.hpp"
#include "filament-sliding/MobileMicrotubule.hpp"
#include "filament-sliding/PossibleFullConnection.hpp"

/* SystemState is a class that keeps track of the current state of the system,
 * and manages the existence of Microtubules and Crosslinkers through its
 * members. SystemState methods allow the SystemState to be changed; they do not
 * provide the rules by which it is changed. Additionally, it gives the force
 * that is on the mobile microtubule, and keeps track of the possible reactions
 * that can happen for certain (un)bindings and hoppings. The latter happens
 * through the CrosslinkerContainers.
 */

class SystemState {
  public:
    enum class ExternalForceType { BARRIERFREE, SINUS, CONSTANT };

  private:
    // With a stretch < 1.5 lattice spacing, there are maximally 3 types of
    // stretch at a time (at exactly 1.5, there could be 4). The number is
    // defined smaller than 1.5 to be sure that there are never 4 states
    // possible, in which the state could become locked. 15 decimals seems to be
    // the precision
    const double m_maxStretchPerLatticeSpacing;
    const int32_t m_maxNumberOfCloseSites;
    const double m_maxStretch; // To be defined in terms of the lattice spacing
                               // and m_maxStretchPerLatticeSpacing
    const double m_latticeSpacing;

    const double m_springConstant;

    Microtubule m_fixedMicrotubule;
    MobileMicrotubule m_mobileMicrotubule;

    const int32_t m_nPassiveCrosslinkers;
    const int32_t m_nDualCrosslinkers;
    const int32_t m_nActiveCrosslinkers;
    const int32_t m_nCrosslinkers;

    CrosslinkerContainer m_passiveCrosslinkers;
    CrosslinkerContainer m_dualCrosslinkers;
    CrosslinkerContainer m_activeCrosslinkers;

    // The force on the microtubule is a property of the system as a whole,
    // because it is caused by the crosslinkers and a possible external force
    double m_forceMicrotubule;
    double m_energy;
    double m_totalExtensionLinkers;

    const bool m_addExternalForce;
    ExternalForceType m_externalForceType; // not const, has to be found in the
                                           // constructor body
    const double m_externalForceValue;

    double externalForceFlatOptimalPath() const;

    const double m_pi = std::acos(-1); // used for the calculation of sinus, for
                                       // a sinusoidal external force
  public:
    SystemState(
            const double lengthMobileMicrotubule,
            const double lengthFixedMicrotubule,
            const double latticeSpacing,
            const double maxStretchPerLatticeSpacing,
            const int32_t nActiveCrosslinkers,
            const int32_t nDualCrosslinkers,
            const int32_t nPassiveCrosslinkers,
            const double springConstant,
            const bool addExternalForce,
            const std::string externalForceTypeString,
            const double externalForceValue);
    ~SystemState();

    SystemState(const SystemState&) = delete;
    SystemState& operator=(const SystemState&) = delete;

    void setMicrotubulePosition(const double positionMicrotubule);

    void fullyConnectFreeCrosslinker(
            const Crosslinker::Type type,
            const Crosslinker::Terminus terminusToConnectToFixedMicrotubule,
            const int32_t positionOnFixedMicrotubule,
            const int32_t positionOnMobileMicrotubule);

    Crosslinker& connectFreeCrosslinker(
            const Crosslinker::Type type,
            const Crosslinker::Terminus terminusToConnect,
            const SiteLocation locationToConnectTo);

    void disconnectPartiallyConnectedCrosslinker(Crosslinker& crosslinker);

    void connectPartiallyConnectedCrosslinker(
            Crosslinker& crosslinker,
            const SiteLocation locationOppositeMicrotubule);

    void disconnectFullyConnectedCrosslinker(
            Crosslinker& crosslinker,
            const Crosslinker::Terminus terminusToDisconnect);

    void updateMobilePosition(const double changeMicrotubulePosition);

    int32_t barrierCrossed();

    std::pair<double, double> movementBordersSetByFullLinkers() const;

    double getMicrotubulePosition() const;

    int32_t getNFreeCrosslinkersOfType(const Crosslinker::Type type) const;
    int32_t getNFreeCrosslinkers() const;

    int32_t getNPartialCrosslinkersOfType(const Crosslinker::Type type) const;
    std::pair<int32_t, int32_t>
    getNPartialCrosslinkersBoundWithHeadAndTailOfType(
            const Crosslinker::Type type) const;

    int32_t getNFullCrosslinkers() const;
    int32_t getNFullRightPullingCrosslinkers() const;

    // The following functions return the position of the overlap as measured
    // from the origin at the beginning of the fixed microtubule
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

    // whichFreeSite gives the location in the storage of the Microtubule
    // objects, does not say anything about which physical site that is.
    int32_t getFreeSitePosition(
            const MicrotubuleType microtubuleType,
            const int32_t whichFreeSite) const;

    double getMaxStretch() const;

#ifdef MYDEBUG
    int32_t getNSitesToBindPartial(const Crosslinker::Type type) const;
#endif // MYDEBUG

    void findPossibilities(const Crosslinker::Type type);

    void updateForceAndEnergy();

    double getForce() const;

    double getEnergy() const;

    double getTotalExtensionLinkers() const;

    double findExternalForce() const;

#ifdef MYDEBUG
    void TESTunbindAFullCrosslinker(
            const int32_t which,
            const Crosslinker::Terminus terminusToDisconnect,
            const Crosslinker::Type typeToDisconnect);
#endif // MYDEBUG

    const std::vector<PossibleFullConnection>& getPossibleConnections(
            const Crosslinker::Type type) const;

    const std::vector<PossiblePartialHop>& getPossiblePartialHops(
            const Crosslinker::Type type) const;

    const std::vector<PossibleFullHop>& getPossibleFullHops(
            const Crosslinker::Type type) const;

    const std::vector<FullConnection>& getFullConnections(
            const Crosslinker::Type type) const;

    const std::vector<Crosslinker*>& getPartialLinkers(
            const Crosslinker::Type type) const;

    const std::vector<Crosslinker*>& getFullLinkers(
            const Crosslinker::Type type) const;

    const std::vector<Crosslinker*>& getPartialLinkersBoundWithHead(
            const Crosslinker::Type type) const;

    const std::vector<Crosslinker*>& getPartialLinkersBoundWithTail(
            const Crosslinker::Type type) const;

    int32_t getNSites(const MicrotubuleType microtubule) const;

    double getLatticeSpacing() const;
};

#endif // SYSTEMSTATE_HPP
