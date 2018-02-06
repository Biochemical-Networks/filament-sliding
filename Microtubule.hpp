#ifndef MICROTUBULE_HPP
#define MICROTUBULE_HPP
#include <cstdint>
#include <vector>
#include <deque>

#include "Input/Input.hpp"
#include "Site.hpp"
#include "Crosslinker.hpp"
#include "PossibleFullConnection.hpp"

class Microtubule
{
private:
    const double m_length;
    const double m_latticeSpacing;
    const int32_t m_nSites;

    // Choose to focus on free instead of occupied sites, since functions may ask if the site is free, not if it is occupied (otherwise completely equivalent of course)
    int32_t m_nFreeSites;
    std::vector<Site> m_sites; // Vector, because the size never changes, but is only known at run time

    std::deque<int32_t> m_freeSitePositions; // Elements are removed often, so std::deque is used. The order of the positions will NOT be preserved.

/*    // Following maps site position to pointer to next free site on the left or right
    std::map<int32_t, Site*> m_nextFreeSiteLeft;
    std::map<int32_t, Site*> m_nextFreeSiteRight;*/

public:

    Microtubule(const double length, const double latticeSpacing);
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

    std::vector<possibleFullConnection> getFreeSitesCloseTo(const double position, const double maxStretch) const;

};

#endif // MICROTUBULE_HPP
