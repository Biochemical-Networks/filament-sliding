#ifndef MICROTUBULE_HPP
#define MICROTUBULE_HPP
#include <cstdint>
#include <vector>
#include <deque>
#include <utility>

#include "Input/Input.hpp"
#include "Site.hpp"
#include "Crosslinker.hpp"
#include "PossibleFullConnection.hpp"
#include "PossibleHop.hpp"
#include "MicrotubuleType.hpp"
#include "FullConnection.hpp"

/* The microtubule class holds data about the microtubule, such as the length, lattice spacing etc. Further, it contains
 * all Sites on the microtubule. There are functions to find the closest sites next to a specific location.
 * Also, it keeps track of the remaining free sites, such that we know where crosslinkers can bind.
 * Finally, it has functions for adding possible connections or hops, since it knows which sites are free, and it can find and remove which
 * possible full connections cross existing full connections.
 */

class Microtubule
{
private:
    const MicrotubuleType m_type;
    double m_length;
protected: // such that MobileMicrotubule can access it
    const double m_latticeSpacing;
private:
    int32_t m_nSites;

    int32_t m_nUnblockedSites;

    // Choose to focus on free instead of occupied sites, since functions may ask if the site is free, not if it is occupied (otherwise completely equivalent of course)
    int32_t m_nFreeSites;
    std::vector<Site> m_sites; // Vector, because the size never changes, but is only known at run time

    std::deque<int32_t> m_freeSitePositions; // Elements are removed often, so std::deque is used. The order of the positions will NOT be preserved.
    std::deque<int32_t> m_unblockedSitePositions;

    std::pair<double,double> getOldAndNewStretchFullHop(const int32_t oldPosition, const int32_t newPosition, const double positionOppositeExtremity, const double maxStretch) const;

    void cleanPossibleCrossings(std::vector<PossibleFullConnection>& possibleConnectionsToCheck, const double mobilePosition, const double maxStretch) const;

    std::vector<FullConnectionLocations> getFullCrosslinkersCloseTo(const double position, const double maxStretch) const;

public:

    Microtubule(const MicrotubuleType type, const double length, const double latticeSpacing);
    virtual ~Microtubule();

    void connectSite(const int32_t sitePosition, Crosslinker& crosslinkerToConnect, const Crosslinker::Terminus terminusToConnect);

    void disconnectSite(const int32_t sitePosition);

    void blockSite(const int32_t sitePosition);

    void unblockSite(const int32_t sitePosition);

    double getLength() const;

    int32_t getNSites() const;

    double getLatticeSpacing() const;

    int32_t getNFreeSites() const;

    int32_t getFreeSitePosition(const int32_t whichFreeSite) const; // whichFreeSite labels the free sites, and can be 0 <= whichFreeSite < m_nFreeSites

    int32_t getUnblockedSitePosition(const int32_t whichUnblockedSite) const;

    int32_t getFirstPositionCloseTo(const double position, const double maxStretch) const;

    int32_t getLastPositionCloseTo(const double position, const double maxStretch) const;

    int32_t getNFreeSitesCloseTo(const double position, const double maxStretch) const;

    // The following functions are const, since they do not modify the Microtubule in any way; only the CrosslinkerContainer is changed
    void addPossibleConnectionsCloseTo(std::vector<PossibleFullConnection>& possibleConnections, Crosslinker* const p_oppositeCrosslinker,
                                       const double position, const double mobilePosition, const double maxStretch) const;

    void addPossiblePartialHopsCloseTo(std::vector<PossiblePartialHop>& possiblePartialHops, Crosslinker* const p_partialLinker) const;

    void addPossibleFullHopsCloseTo(std::vector<PossibleFullHop>& possibleFullHops, const FullExtremity& fullLinkerExtremity,
                                    const double positionOppositeExtremity, const double maxStretch) const;

    std::vector<Crosslinker*> getPartialCrosslinkersCloseTo(const double position, const double maxStretch, const Crosslinker::Type typeToCheck) const;

    // The following functions take a location instead of a linker (pointer), since it could be used for finding neighbours of both full or partial linkers
    std::vector<Crosslinker*> getNeighbouringPartialCrosslinkersOf(const SiteLocation& originLocation, const Crosslinker::Type typeToCheck) const;

    std::vector<FullExtremity> getNeighbouringFullCrosslinkersOf(const SiteLocation& originLocation, const Crosslinker::Type typeToCheck) const;

    void growOneSite();

    int32_t getNUnblockedSites() const;

};

#endif // MICROTUBULE_HPP
