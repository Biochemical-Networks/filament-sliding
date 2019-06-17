#ifndef CROSSLINKERCONTAINER_HPP
#define CROSSLINKERCONTAINER_HPP

#include "Crosslinker.hpp"
#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"
#include "PossibleFullConnection.hpp"
#include "FullConnection.hpp"

#include <cstdint>
#include <vector>
#include <utility> // for std::pair

/* CrosslinkerContainer has two main functions:
 * First, it holds the Crosslinkers of one type, and actively changes their state when called to do so through (dis)connect methods.
 * Second, it finds and stores information about the possible reactions that could happen.
 * The latter is done through reset and update functions, and these have to be called explicitly after the system has changed,
 * because a change in e.g. one crosslinker can cause changes to the possibilities in all types of crosslinkers, in each connecting configuration.
 */

class CrosslinkerContainer
{
private:
    Crosslinker::Type m_linkerType;
    std::vector<Crosslinker> m_crosslinkers; // The vector manages existence of the crosslinkers. In the current version at least, do not resize this

    // Since this class holds many functions checking which modifications are possible, it needs to know the following quantities often
    const Microtubule& m_fixedMicrotubule;
    const MobileMicrotubule& m_mobileMicrotubule;
    const double m_latticeSpacing;
    const double m_maxStretch;
    // The following are the points on the lattice (k*latticeSpacing + mod, with k = integer) where possible connections may change when the mobile microtubule moves past it
    const double m_mod1, m_mod2;
    double m_lowerBorderPossibilities, m_upperBorderPossibilities; // These are the borders for the mobile position between which the current possibilities remain valid

    // Use pointers to crosslinkers as IDs: THIS IS DANGEROUS! It will break if m_crosslinkers would ever resize, since that invalidates all pointers to its elements.
    // A possible fix for this (if it were required to resize the m_crosslinkers sometimes) would be to store the labels (0,...,nCrosslinkers-1) instead of pointers).
    // Not the case for now
    std::vector<Crosslinker*> m_freeCrosslinkers;
    std::vector<Crosslinker*> m_partialCrosslinkers;
    std::vector<Crosslinker*> m_fullCrosslinkers;
    // Keep track of the partial linkers that are bound to the tip or to the blocked region
    std::vector<Crosslinker*> m_partialCrosslinkersOnTip;
    std::vector<Crosslinker*> m_partialCrosslinkersOnBlocked;

    // Stores all possible connections such that the search needs to be done once every time step
    // Needs to be updated dynamically.  After MT diffusion, it can completely change
    // Will be traversed linearly, to calculate all rates: this should happen every time step, so a vector can be accessed quickly.
    // However, to find elements or for resizing, it may be less quick. Make sure that it doesn't resize too often!
    // Added to crosslinkerContainer, because it is stored for each type of crosslinker separately.
    // Not separate for Tip or Blocked, since full binding rates are independent from the MT state
    std::vector<PossibleFullConnection> m_possibleConnections;

    /*std::vector<PossiblePartialHop> m_possiblePartialHops;

    std::vector<PossibleFullHop> m_possibleFullHops;*/

    // Keep track of full connections, indepependent from Tip or Blocked states of the microtubule site connected to.
    // The binding / unbinding of full crosslinks does not change with the MT state, only the (un)binding of partial linkers changes.
    std::vector<FullConnection> m_fullConnections; // These are not possibilities, but actual connections


    // The following functions are used internally; cannot be called by public, m_possibleConnections is only altered through calls to (dis)connect functions, or to findPossibleConnections
    // add and remove functions are concerned with adding or removing all (possible) connections of a specific crosslinker (extremity).
    void addPossibleConnections(Crosslinker*const p_newPartialCrosslinker);

    void removePossibleConnections(Crosslinker*const p_oldPartialCrosslinker);

    /*void addPossiblePartialHops(Crosslinker*const p_newPartialCrosslinker);

    void removePossiblePartialHops(Crosslinker*const p_oldPartialCrosslinker);

    void addPossibleFullHops(Crosslinker*const p_newFullCrosslinker); // Hops are implemented as a combined unbinding-binding event, so the possibilities are updated through an unbinding

    void removePossibleFullHops(Crosslinker*const p_oldFullCrosslinker);*/

    void addFullConnection(Crosslinker*const p_newFullCrosslinker);

    void removeFullConnection(Crosslinker*const p_oldFullCrosslinker);

    // The following update functions change the possibilities for partial and full linkers in the surroundings of a site that underwent a change

    void updatePossibleConnectionsOppositeTo(Crosslinker*const p_partialCrosslinker, SiteLocation locationConnection);

    /*void updatePossiblePartialHopsNextTo(const SiteLocation& originLocation);

    void updatePossibleFullHopsNextTo(const SiteLocation& originLocation);*/

    // The findPossible... functions recalculate the possibility vectors,
    // while the findPossibilityBorders calculates and sets the borders within which the current possibilities are valid.
    // When the mobileMicrotubule.position leaves the region set by these borders, the possibilities need to be recalculated

    void findPossibilityBorders();

    void findPossibleConnections();

    /*void findPossiblePartialHops();

    void findPossibleFullHops();*/

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

    // The following four functions correspond to the reactions free<->partial<->full.
    // These store the actual changes made, and change the state of the system.
    // There are four other functions (updateConnectionData functions) that do the updating of possibilities after the system has reached its new state
    // Notice there are no functions for hopping (nor are there updateConnectionData functions for hopping). This is because a hop is finally implemented as an unbind, rebind sequence.
    // Still, the possibilities need to be tracked, since hopping is a separate reaction.
    Crosslinker* connectFromFreeToPartial(const SiteType siteTypeBoundTo);

    void disconnectFromPartialToFree(Crosslinker& crosslinkerToDisconnect);

    void connectFromPartialToFull(Crosslinker& crosslinkerToConnect);

    void disconnectFromFullToPartial(Crosslinker& crosslinkerToDisconnect);

    void blockConnectedSite(Crosslinker& crosslinkerToBlock);

    int32_t getNCrosslinkers() const;
    int32_t getNFreeCrosslinkers() const;
    int32_t getNPartialCrosslinkers(const SiteType siteType) const;
    int32_t getNFullCrosslinkers() const;

    int32_t getNFullRightPullingCrosslinkers() const;

    #ifdef MYDEBUG
    int32_t getNSitesToBindPartial() const; // Function to test whether algorithm works
    #endif // MYDEBUG

    // Recalculates all possibilities: possible bindings partial->full, possible hops for partials and fulls, and sets the movement boundaries after which possibilities have to be reset again.
    void resetPossibilities();

    // The update functions have two purposes: to add possibilities for the new state of the linker, and to update the possibilities of the surrounding linkers.
    // They need to be called when any linker goes through the change: when the linker is of a different type, it can still affect the possibilities of linkers of this type
    void updateConnectionDataFreeToPartial(Crosslinker*const p_newPartialCrosslinker);

    void updateConnectionDataPartialToFree(Crosslinker*const p_oldPartialCrosslinker, const SiteLocation locationOldConnection);

    void updateConnectionDataPartialToFull(Crosslinker*const p_newFullCrosslinker, const SiteLocation locationNewConnection);

    void updateConnectionDataFullToPartial(Crosslinker*const p_oldFullCrosslinker, const SiteLocation locationOldConnection);

    void updateConnectionDataMobilePositionChange(const double positionChange);

    void updateConnectionDataMicrotubuleGrowth();

    void updateConnectionDataBlockSite(const int32_t sitePosition);

    std::pair<double, double> movementBordersSetByFullLinkers() const;

    const std::vector<PossibleFullConnection>& getPossibleConnections() const;

    /*const std::vector<PossiblePartialHop>& getPossiblePartialHops() const;

    const std::vector<PossibleFullHop>& getPossibleFullHops() const;*/

    const std::vector<FullConnection>& getFullConnections() const;

    const std::vector<Crosslinker*>& getPartialLinkers() const;

    const std::vector<Crosslinker*>& getPartialLinkers(const SiteType siteType) const; // function overloading

    const std::vector<Crosslinker*>& getFullLinkers() const;

    void checkInternalConsistency(); // on mutable objects, such that reset functions can be used to recalculate
};

#endif // CROSSLINKERCONTAINER_HPP
