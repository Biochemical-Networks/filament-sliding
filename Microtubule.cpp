#include "Microtubule.hpp"
#include <cstdint>
#include <stdexcept>
#include <algorithm> // max/min
#include <cmath> // ceil/floor
#include <vector>

#include "PossibleFullConnection.hpp"
#include "PossibleHop.hpp"
#include "MicrotubuleType.hpp"

#include "Site.hpp"
#include "Crosslinker.hpp"


Microtubule::Microtubule(const MicrotubuleType type, const double length, const double latticeSpacing)
    :   m_type(type),
        m_length(length),
        m_latticeSpacing(latticeSpacing),
        m_nSites(static_cast<int32_t>(std::floor(m_length/m_latticeSpacing))+1), // Choose such that microtubule always starts and ends with a site. The
        m_nFreeSites(m_nSites),
        m_sites(m_nSites, Site(true)), // Create a copy of Site which is free (isFree=true), and copy it into the vector
        m_freeSitePositions(m_nSites) // Set the number of sites, but fill it in the body of the constructor
{
    std::iota(m_freeSitePositions.begin(), m_freeSitePositions.end(), 0); // All sites are initially free
}

Microtubule::~Microtubule()
{
}

void Microtubule::connectSite(const int32_t sitePosition, Crosslinker& crosslinkerToConnect, const Crosslinker::Terminus terminusToConnect)
{
    try
    {
        m_sites.at(sitePosition).connectCrosslinker(crosslinkerToConnect, terminusToConnect);

        m_freeSitePositions.erase(std::remove(m_freeSitePositions.begin(), m_freeSitePositions.end(), sitePosition), m_freeSitePositions.end()); // Erase-remove idiom
        --m_nFreeSites;

    }
    catch(std::out_of_range)
    {
        throw GeneralException("The sitePosition given to Microtubule::connectSite() does not exist");
    }
}

void Microtubule::disconnectSite(const int32_t sitePosition)
{
    try
    {
        m_sites.at(sitePosition).disconnectCrosslinker();

        m_freeSitePositions.push_back(sitePosition);
        ++m_nFreeSites;

    }
    catch(std::out_of_range)
    {
        throw GeneralException("The sitePosition given to Microtubule::disconnectSite() does not exist");
    }
}


double Microtubule::getLength() const
{
    return m_length;
}

int32_t Microtubule::getNSites() const
{
    return m_nSites;
}

double Microtubule::getLatticeSpacing() const
{
    return m_latticeSpacing;
}

int32_t Microtubule::getNFreeSites() const
{
    return m_nFreeSites;
}

int32_t Microtubule::getFreeSitePosition(const int32_t whichFreeSite) const
{
    return m_freeSitePositions.at(whichFreeSite);
}

// The function floor((x-maxStretch)/latticeSpacing+1) returns the first point that is within a distance of maxStretch from x, excluding the exact distance of maxStretch.
// Given A<C, min(max(A,B),C) = { A if B<=A, B if A<=B<=C, C if B>=C }. (By the way, if A<C, then min(max(A,B),C)=max(A,min(B,C)) )
int32_t Microtubule::getFirstPositionCloseTo(const double position, const double maxStretch) const
{
    int32_t estimatedSite = static_cast <int32_t> (std::floor( (position-maxStretch)/m_latticeSpacing+1));
    // The first position cannot be smaller than 0, and not bigger than (NSites-1), since counting starts at 0
    return std::min(std::max(static_cast <int32_t> (0), estimatedSite),m_nSites-1);
}

int32_t Microtubule::getLastPositionCloseTo(const double position, const double maxStretch) const
{
    int32_t estimatedSite = static_cast <int32_t> (std::ceil( (position+maxStretch)/m_latticeSpacing-1));
    // The first position cannot be smaller than 0, and not bigger than (NSites-1), since counting starts at 0
    return std::min(std::max(static_cast <int32_t> (0), estimatedSite),m_nSites-1);
}

int32_t Microtubule::getNFreeSitesCloseTo(const double position, const double maxStretch) const
{
    if (position<=-maxStretch||position >= m_length + maxStretch)
    {
        return 0;
    }
    else
    {
        // Now, we can assume there is at least one site (does not have to be free) within reach
        int32_t nFreeSites = 0;
        int32_t lowerSiteLabel = getFirstPositionCloseTo(position, maxStretch);
        int32_t upperSiteLabel = getLastPositionCloseTo(position, maxStretch);
        for (int32_t posToCheck = lowerSiteLabel; posToCheck<=upperSiteLabel; ++posToCheck)
        {
            if(m_sites.at(posToCheck).isFree())
            {
                ++nFreeSites;
            }
        }
        return nFreeSites;
    }
}

// position is the position relative to the start of THIS microtubule, not the mobile one per se
void Microtubule::addPossibleConnectionsCloseTo(std::vector<PossibleFullConnection>& possibleConnections,
                                                Crosslinker* const p_oppositeCrosslinker,
                                                const double position,
                                                const double maxStretch) const
{
    if(!p_oppositeCrosslinker->isPartial())
    {
        throw GeneralException("Microtubule::addPossibleConnectionsCloseTo() encountered a non-partial linker.");
    }

    if (!(position<=-maxStretch||position >= m_length + maxStretch)) // Definitely no sites close if there is no microtubule there
    {
        // Now, we can assume there is at least one site (does not have to be free) within reach
        int32_t lowerSiteLabel = getFirstPositionCloseTo(position, maxStretch);
        int32_t upperSiteLabel = getLastPositionCloseTo(position, maxStretch);
        for (int32_t posToCheck = lowerSiteLabel; posToCheck<=upperSiteLabel; ++posToCheck)
        {
            /* The stretch is defined as the position of the connection on the mobile microtubule,
             * minus the position on the fixed microtubule. The sign matters!
             * This is done such that the stretch can be easily updated after a change in the microtubule position
             */
            if(m_sites.at(posToCheck).isFree())
            {
                double stretch;
                switch(m_type)
                {
                case MicrotubuleType::FIXED:
                    stretch = position - posToCheck*m_latticeSpacing;
                    break;
                case MicrotubuleType::MOBILE:
                    stretch = posToCheck*m_latticeSpacing - position;
                    break;
                default:
                    throw GeneralException("Wrong microtubule type in Microtubule::addPossibleConnectionsCloseTo()");
                }
                possibleConnections.push_back(PossibleFullConnection{p_oppositeCrosslinker, SiteLocation{m_type, posToCheck}, stretch});
            }
        }
    }
}

void Microtubule::addPossiblePartialHopsCloseTo(std::vector<PossiblePartialHop>& possiblePartialHops, Crosslinker* const p_partialLinker) const
{
    if(!p_partialLinker->isPartial())
    {
        throw GeneralException("Microtubule::addPossiblePartialHopsCloseTo() encountered a non-partial linker.");
    }

    SiteLocation partialLocation = p_partialLinker->getBoundLocationWhenPartiallyConnected();

    #ifdef MYDEBUG
    if (partialLocation.microtubule != m_type)
    {
        throw GeneralException("Microtubule::addPossiblePartialHopsCloseTo() was called on the wrong microtubule.");
    }
    #endif // MYDEBUG

    if(partialLocation.position!=0 && m_sites.at(partialLocation.position-1).isFree())
    {
        possiblePartialHops.push_back(PossiblePartialHop{p_partialLinker, SiteLocation{m_type, partialLocation.position-1}});
    }
    if(partialLocation.position != (m_nSites-1) && m_sites.at(partialLocation.position+1).isFree())
    {
        possiblePartialHops.push_back(PossiblePartialHop{p_partialLinker, SiteLocation{m_type, partialLocation.position+1}});
    }
}

std::vector<Crosslinker*> Microtubule::getPartialCrosslinkersCloseTo(const double position, const double maxStretch, const Crosslinker::Type typeToCheck) const
{
    if (position<=-maxStretch||position >= m_length + maxStretch) // No sites close to a point outside of the microtubule
    {
        return {}; // empty vector
    }
    else
    {
        // Now, we can assume there is at least one site (does not have to be free) within reach
        int32_t lowerSiteLabel = getFirstPositionCloseTo(position, maxStretch);
        int32_t upperSiteLabel = getLastPositionCloseTo(position, maxStretch);
        std::vector<Crosslinker*> partialsCloseby;
        for (int32_t posToCheck = lowerSiteLabel; posToCheck<=upperSiteLabel; ++posToCheck)
        {
            if(( m_sites.at(posToCheck).isPartial()) && (m_sites.at(posToCheck).whichCrosslinkerIsBound()->getType()==typeToCheck) )
            {
                partialsCloseby.push_back(m_sites.at(posToCheck).whichCrosslinkerIsBound());
            }
        }
        return partialsCloseby;
    }
}




