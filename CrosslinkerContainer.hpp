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

    std::vector<FullConnection> m_fullConnections;

    // The following functions are used internally; cannot be called by public, m_possibleConnections is only altered through calls to (dis)connect functions, or to findPossibleConnections
    void addPossibleConnections(Crosslinker*const p_newPartialCrosslinker,
                                const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing);

    void removePossibleConnections(Crosslinker*const p_oldPartialCrosslinker);

    void updatePossibleConnectionsOppositeTo(Crosslinker*const p_partialCrosslinker, SiteLocation locationConnection,
                                             const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing);

    void addFullConnection(Crosslinker*const p_newFullCrosslinker, const double mobilePosition, const double latticeSpacing, const double maxStretch);

    void removeFullConnection(Crosslinker*const p_oldFullCrosslinker);

    bool partialPossibleConnectionsConformToMobilePositionChange(const double positionChange, const double maxStretch) const;

public:
    CrosslinkerContainer(const int32_t nCrosslinkers, const Crosslinker& defaultCrosslinker, const Crosslinker::Type linkerType);
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
    int32_t getNSitesToBindPartial(const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing) const;
    #endif // MYDEBUG

    void findPossibleConnections(const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing);

    void updateConnectionDataFreeToPartial(Crosslinker*const p_newPartialCrosslinker,
                                                const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing);

    void updateConnectionDataPartialToFree(Crosslinker*const p_oldPartialCrosslinker, const SiteLocation locationOldConnection,
                                               const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing);

    void updateConnectionDataPartialToFull(Crosslinker*const p_newPartialCrosslinker, const SiteLocation locationNewConnection,
                                                const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing);

    void updateConnectionDataFullToPartial(Crosslinker*const p_oldPartialCrosslinker, const SiteLocation locationOldConnection,
                                                const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing);

    std::pair<double, double> movementBordersSetByFullLinkers(const double maxStretch) const;

    void updateConnectionsAfterMobilePositionChange(const double positionChange,
                                                    const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing);

    const std::vector<PossibleFullConnection>& getPossibleConnections() const;

    const std::vector<FullConnection>& getFullConnections() const;

    const std::vector<Crosslinker*>& getPartialLinkers() const;


    #ifdef MYDEBUG
    Crosslinker* TESTgetAFullCrosslinker(const int32_t which) const;
    #endif // MYDEBUG
};

#endif // CROSSLINKERCONTAINER_HPP
