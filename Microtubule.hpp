#ifndef MICROTUBULE_HPP
#define MICROTUBULE_HPP
#include <cstdint>
#include <vector>
#include <deque>

#include "Input/Input.hpp"
#include "Site.hpp"
#include "Crosslinker.hpp"
#include "PossibleFullConnection.hpp"
#include "MicrotubuleType.hpp"

class Microtubule
{
private:
    const MicrotubuleType m_type;
    const double m_length;
    const double m_latticeSpacing;
    const int32_t m_nSites;

    // Choose to focus on free instead of occupied sites, since functions may ask if the site is free, not if it is occupied (otherwise completely equivalent of course)
    int32_t m_nFreeSites;
    std::vector<Site> m_sites; // Vector, because the size never changes, but is only known at run time

    std::deque<int32_t> m_freeSitePositions; // Elements are removed often, so std::deque is used. The order of the positions will NOT be preserved.

public:

    Microtubule(const MicrotubuleType type, const double length, const double latticeSpacing);
    virtual ~Microtubule();

    void connectSite(const int32_t sitePosition, Crosslinker& crosslinkerToConnect, const Crosslinker::Terminus terminusToConnect);

    void disconnectSite(const int32_t sitePosition);

    double getLength() const;

    int32_t getNSites() const;

    double getLatticeSpacing() const;

    int32_t getNFreeSites() const;

    int32_t getFreeSitePosition(const int32_t whichFreeSite) const; // whichFreeSite labels the free sites, and can be 0 <= whichFreeSite < m_nFreeSites

    int32_t getFirstPositionCloseTo(const double position, const double maxStretch) const;

    int32_t getLastPositionCloseTo(const double position, const double maxStretch) const;

    int32_t getNFreeSitesCloseTo(const double position, const double maxStretch) const;

    // The following functions are const, since they do not modify the Microtubule in any way; only the CrosslinkerContainer is changed
    void addFreeSitesCloseTo(std::vector<PossibleFullConnection>& possibleConnections, Crosslinker* const oppositeCrosslinker, const double position, const double maxStretch) const;

    void removePossibleConnectionsCloseTo(std::vector<PossibleFullConnection>& possibleConnections, Crosslinker* const oppositeCrosslinker, const double position, const double maxStretch) const;

};

#endif // MICROTUBULE_HPP
