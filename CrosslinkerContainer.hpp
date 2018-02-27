#ifndef CROSSLINKERCONTAINER_HPP
#define CROSSLINKERCONTAINER_HPP

#include "Crosslinker.hpp"
#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "PossibleFullConnection.hpp"
#include "FullConnection.hpp"
#include "PossibleHop.hpp"

#include <cstdint>
#include <vector>
#include <utility> // for std::pair

class CrosslinkerContainer
{
private:
    Crosslinker::Type m_linkerType;
    std::vector<Crosslinker> m_crosslinkers; // The vector manages existence of the crosslinkers.

    // Since this class holds many functions checking which modifications are possible, it needs to know the following quantities often
    const Microtubule& m_fixedMicrotubule;
    const MobileMicrotubule& m_mobileMicrotubule;
    const double m_latticeSpacing;
    const double m_maxStretch;
    const double m_mod1, m_mod2; // These are the points on the lattice (k*latticeSpacing + mod, with k = integer) where possible connections may change
    double m_lowerBorderPossibilities, m_upperBorderPossibilities;


    int32_t m_nFreeCrosslinkers;
    // Use pointers to crosslinkers as IDs: THIS IS DANGEROUS! It will break if m_crosslinkers would ever resize, since that invalidates all pointers to its elements.
    // A possible fix for this (if it were required to resize the m_crosslinkers sometimes) would be to store the labels (0,...,nCrosslinkers-1) instead of pointers)
    std::vector<Crosslinker*> m_freeCrosslinkers;
    std::vector<Crosslinker*> m_partialCrosslinkers;
    std::vector<Crosslinker*> m_fullCrosslinkers;

    // Stores all possible connections such that the search needs to be done once every time step
    // Needs to be updated dynamically.  After MT diffusion, it can completely change
    // Will be traversed linearly, to calculate all rates: this should happen every time step, so a vector can be accessed quickly.
    // However, to find elements or for resizing, it may be less quick. Make sure that it doesn't resize too often!
    // Added to crosslinkerContainer, because it is stored for each type of crosslinker separately.
    std::vector<PossibleFullConnection> m_possibleConnections;

    std::vector<PossiblePartialHop> m_possiblePartialHops;

    std::vector<PossibleFullHop> m_possibleFullHops;

    std::vector<FullConnection> m_fullConnections; // These are not possibilities, but actual connections

    // The following functions are used internally; cannot be called by public, m_possibleConnections is only altered through calls to (dis)connect functions, or to findPossibleConnections
    void addPossibleConnections(Crosslinker*const p_newPartialCrosslinker);

    void removePossibleConnections(Crosslinker*const p_oldPartialCrosslinker);

    void addPossiblePartialHops(Crosslinker*const p_newPartialCrosslinker);

    void removePossiblePartialHops(Crosslinker*const p_oldPartialCrosslinker);

    void addPossibleFullHops(const FullExtremity& newFullExtremity);

    void removePossibleFullHops(Crosslinker*const p_oldFullCrosslinker);

    void addFullConnection(Crosslinker*const p_newFullCrosslinker);

    void removeFullConnection(Crosslinker*const p_oldFullCrosslinker);

    void updatePossibleConnectionsOppositeTo(Crosslinker*const p_partialCrosslinker, SiteLocation locationConnection);

    void updatePossiblePartialHopsNextTo(const SiteLocation& originLocation);

    void updatePossibleFullHopsNextTo(const SiteLocation& originLocation);

    double myMod(const double x, const double y) const; // used in findPossibilityBorders()

    void findPossibilityBorders();

    void findPossibleConnections();

    void findPossiblePartialHops();

    void findPossibleFullHops();

public:
    CrosslinkerContainer(const int32_t nCrosslinkers,
                         const Crosslinker& defaultCrosslinker,
                         const Crosslinker::Type linkerType,
                         const Microtubule& fixedMicrotubule,
                         const MobileMicrotubule& mobileMicrotubule,
                         const double latticeSpacing,
                         const double maxStretch);
    ~CrosslinkerContainer();
    // Delete the default copy constructor and assignment operator, there is no use for them
    CrosslinkerContainer(const CrosslinkerContainer&) = delete;
    CrosslinkerContainer& operator=(const CrosslinkerContainer&) = delete;

    Crosslinker& at(const int32_t position);

    Crosslinker* connectFromFreeToPartial();

    void disconnectFromPartialToFree(Crosslinker& crosslinkerToDisconnect);

    void connectFromPartialToFull(Crosslinker& crosslinkerToConnect);

    void disconnectFromFullToPartial(Crosslinker& crosslinkerToDisconnect);

    int32_t getNCrosslinkers() const;
    int32_t getNFreeCrosslinkers() const;
    int32_t getNPartialCrosslinkers() const;
    int32_t getNFullCrosslinkers() const;

    #ifdef MYDEBUG
    int32_t getNSitesToBindPartial() const;
    #endif // MYDEBUG

    void resetPossibilities();

    // The update functions have two purposes: to add possibilities for the new state of the linker, and to update the possibilities of the surrounding linkers.
    void updateConnectionDataFreeToPartial(Crosslinker*const p_newPartialCrosslinker);

    void updateConnectionDataPartialToFree(Crosslinker*const p_oldPartialCrosslinker, const SiteLocation locationOldConnection);

    void updateConnectionDataPartialToFull(Crosslinker*const p_newPartialCrosslinker, const SiteLocation locationNewConnection);

    void updateConnectionDataFullToPartial(Crosslinker*const p_oldPartialCrosslinker, const SiteLocation locationOldConnection);

    std::pair<double, double> movementBordersSetByFullLinkers() const;

    void updateConnectionsAfterMobilePositionChange(const double positionChange);

    const std::vector<PossibleFullConnection>& getPossibleConnections() const;

    const std::vector<FullConnection>& getFullConnections() const;

    const std::vector<Crosslinker*>& getPartialLinkers() const;


    #ifdef MYDEBUG
    Crosslinker* TESTgetAFullCrosslinker(const int32_t which) const;
    #endif // MYDEBUG
};

#endif // CROSSLINKERCONTAINER_HPP
