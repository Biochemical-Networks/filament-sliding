#include "Microtubule.hpp"
#include <cstdint>
#include <stdexcept>
#include <algorithm> // max/min
#include <cmath> // ceil/floor
#include <vector>
#include <utility> // pair
#include <numeric>

#include "PossibleFullConnection.hpp"
#include "PossibleHop.hpp"
#include "MicrotubuleType.hpp"

#include "Site.hpp"
#include "Crosslinker.hpp"


Microtubule::Microtubule(const MicrotubuleType type, const double length, const double latticeSpacing)
    :   m_type(type),
        m_length(length),
        m_latticeSpacing(latticeSpacing),
        m_nSites(static_cast<int32_t>(std::floor(m_length/m_latticeSpacing))+1), // Choose such that microtubule always starts and ends with a site
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
    #ifdef MYDEBUG
    try
    {
    #endif // MYDEBUG
        m_sites.at(sitePosition).connectCrosslinker(crosslinkerToConnect, terminusToConnect);

        m_freeSitePositions.erase(std::remove(m_freeSitePositions.begin(), m_freeSitePositions.end(), sitePosition), m_freeSitePositions.end()); // Erase-remove idiom
        --m_nFreeSites;
    #ifdef MYDEBUG
    }
    catch(std::out_of_range)
    {
        throw GeneralException("The sitePosition given to Microtubule::connectSite() does not exist");
    }
    #endif // MYDEBUG
}

void Microtubule::disconnectSite(const int32_t sitePosition)
{
    #ifdef MYDEBUG
    try
    {
    #endif // MYDEBUG
        m_sites.at(sitePosition).disconnectCrosslinker();

        m_freeSitePositions.push_back(sitePosition);
        ++m_nFreeSites;
    #ifdef MYDEBUG
    }
    catch(std::out_of_range)
    {
        throw GeneralException("The sitePosition given to Microtubule::disconnectSite() does not exist");
    }
    #endif // MYDEBUG
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
    #ifdef MYDEBUG
    if (whichFreeSite<0 || whichFreeSite >= m_nFreeSites)
    {
        throw GeneralException("Microtubule::getFreeSitePosition() was called with an invalid parameter");
    }
    #endif // MYDEBUG
    return m_freeSitePositions.at(whichFreeSite);
}

// The function floor((x-maxStretch)/latticeSpacing+1) returns the first point that is within a distance of maxStretch from x, excluding the exact distance of maxStretch.
// The function ceil((x+maxStretch)/latticeSpacing-1) returns the last point that is within a distance of maxStretch from x, excluding the exact distance of maxStretch.
// Given A<=C, min(max(A,B),C) = { A if B<=A, B if A<=B<=C, C if B>=C }. (By the way, if A<=C, then min(max(A,B),C)=max(A,min(B,C)) )
int32_t Microtubule::getFirstPositionCloseTo(const double position, const double maxStretch) const
{
    int32_t estimatedSite = static_cast <int32_t> (std::floor( (position-maxStretch)/m_latticeSpacing+1));
    // The first position cannot be smaller than 0, and not bigger than (NSites-1), since counting starts at 0
    return std::min(std::max(static_cast <int32_t> (0), estimatedSite),m_nSites-1); // m_nSites >=1, so this is always good
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
                                                const double mobilePosition,
                                                const double maxStretch) const
{
    #ifdef MYDEBUG
    if(!p_oppositeCrosslinker->isPartial())
    {
        throw GeneralException("Microtubule::addPossibleConnectionsCloseTo() encountered a non-partial linker.");
    }
    if(p_oppositeCrosslinker->getBoundLocationWhenPartiallyConnected().microtubule == m_type)
    {
        throw GeneralException("Microtubule::addPossibleConnectionsCloseTo() was called for a crosslinker on the same microtubule");
    }
    #endif // MYDEBUG

    if (!(position<=-maxStretch||position >= m_length + maxStretch)) // Definitely no sites close if there is no microtubule there
    {
        // Now, we can assume there is at least one site (does not have to be free) within reach
        int32_t lowerSiteLabel = getFirstPositionCloseTo(position, maxStretch);
        int32_t upperSiteLabel = getLastPositionCloseTo(position, maxStretch);
        std::vector<PossibleFullConnection> newPossibleConnections;
        for (int32_t posToCheck = lowerSiteLabel; posToCheck<=upperSiteLabel; ++posToCheck)
        {
            // The stretch is defined as the position of the connection on the mobile microtubule,
            // minus the position on the fixed microtubule. The sign matters!
            // This is done such that the stretch can be easily updated after a change in the microtubule position, and the force can be easily calculated
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
                #ifdef MYDEBUG
                if(std::abs(stretch)>=maxStretch)
                {
                    throw GeneralException("Microtubule::addPossibleConnectionsCloseTo() tried to make a disallowed stretch");
                }
                #endif // MYDEBUG

                // Check if the new connection proposed does not cross an existing full connection (we disallow that).
                // For this, make use of the fact that the opposite extremity of that full linker should be within a maxStretch distance compared to the position of the partial linker

                newPossibleConnections.push_back(PossibleFullConnection{p_oppositeCrosslinker, SiteLocation{m_type, posToCheck}, stretch});
            }
        }
        cleanPossibleCrossings(newPossibleConnections, mobilePosition, maxStretch);
        possibleConnections.insert(possibleConnections.end(), newPossibleConnections.begin(), newPossibleConnections.end());
    }
}

void Microtubule::addPossiblePartialHopsCloseTo(std::vector<PossiblePartialHop>& possiblePartialHops, Crosslinker* const p_partialLinker) const
{
    #ifdef MYDEBUG
    if(!p_partialLinker->isPartial())
    {
        throw GeneralException("Microtubule::addPossiblePartialHopsCloseTo() encountered a non-partial linker.");
    }
    #endif // MYDEBUG

    SiteLocation partialLocation = p_partialLinker->getBoundLocationWhenPartiallyConnected();
    Crosslinker::Terminus terminusToHop = p_partialLinker->getBoundTerminusWhenPartiallyConnected();

    #ifdef MYDEBUG
    if (partialLocation.microtubule != m_type)
    {
        throw GeneralException("Microtubule::addPossiblePartialHopsCloseTo() was called on the wrong microtubule.");
    }
    #endif // MYDEBUG

    // For the fixed microtubule, the plus tip is at (m_nSites-1), the minus tip is at 0. For the mobile microtubule this is opposite: plus @ 0, minus @ (m_nSites-1)
    // The hop direction is forward when towards the plus tip, and backwards otherwise
    if(partialLocation.position!=0 && m_sites.at(partialLocation.position-1).isFree())
    {
        HopDirection direction = (m_type==MicrotubuleType::FIXED)?(HopDirection::BACKWARD):(HopDirection::FORWARD);
        possiblePartialHops.push_back(PossiblePartialHop{p_partialLinker, terminusToHop, SiteLocation{m_type, partialLocation.position-1}, direction});
    }
    if(partialLocation.position != (m_nSites-1) && m_sites.at(partialLocation.position+1).isFree())
    {
        HopDirection direction = (m_type==MicrotubuleType::FIXED)?(HopDirection::FORWARD):(HopDirection::BACKWARD);
        possiblePartialHops.push_back(PossiblePartialHop{p_partialLinker, terminusToHop, SiteLocation{m_type, partialLocation.position+1}, direction});
    }
}

// positionOppositeExtremity is the position relative to this microtubule
std::pair<double,double> Microtubule::getOldAndNewStretchFullHop(const int32_t oldPosition, const int32_t newPosition, const double positionOppositeExtremity, const double maxStretch) const
{
    // stretch is mobilePos - fixedPos
    double oldStretch;
    double newStretch;

    switch(m_type)
    {
    case MicrotubuleType::FIXED:
        oldStretch = positionOppositeExtremity - oldPosition*m_latticeSpacing;
        newStretch = positionOppositeExtremity - newPosition*m_latticeSpacing;
        break;
    case MicrotubuleType::MOBILE:
        oldStretch = oldPosition*m_latticeSpacing - positionOppositeExtremity;
        newStretch = newPosition*m_latticeSpacing - positionOppositeExtremity;
        break;
    default:
        throw GeneralException("Microtubule::getOldAndNewStretchFullHop() encountered a wrong microtubule type");
    }

    #ifdef MYDEBUG
    if (std::abs(oldStretch) >= maxStretch)
    {
        throw GeneralException("Microtubule::getOldAndNewStretchFullHop() encountered a forbidden stretch.");
    }
    // The user that calls this function has to decide whether to accept the new stretch, a forbidden newStretch that is returned is taken as a signal that the hop is not possible
    #endif // MYDEBUG

    return std::pair<double,double>{oldStretch, newStretch};
}

// positionOppositeExtremity is the position relative to this microtubule
void Microtubule::addPossibleFullHopsCloseTo(std::vector<PossibleFullHop>& possibleFullHops,
                                             const FullExtremity& fullLinkerExtremity,
                                             const double positionOppositeExtremity,
                                             const double maxStretch) const
{
    #ifdef MYDEBUG
    if(!fullLinkerExtremity.p_fullLinker->isFull())
    {
        throw GeneralException("Microtubule::addPossibleFullHopsCloseTo() encountered a non-full linker.");
    }
    #endif // MYDEBUG

    SiteLocation originLocation = fullLinkerExtremity.p_fullLinker->getOneBoundLocationWhenFullyConnected(fullLinkerExtremity.terminus);

    #ifdef MYDEBUG
    if (originLocation.microtubule != m_type)
    {
        throw GeneralException("Microtubule::addPossibleFullHopsCloseTo() was called on the wrong microtubule.");
    }
    #endif // MYDEBUG

    if(originLocation.position!=0 && m_sites.at(originLocation.position-1).isFree())
    {
        std::pair<double,double> oldAndNewStretch = getOldAndNewStretchFullHop(originLocation.position, originLocation.position-1, positionOppositeExtremity, maxStretch);

        if (std::abs(oldAndNewStretch.second) < maxStretch)
        {
            HopDirection direction = (m_type==MicrotubuleType::FIXED)?(HopDirection::BACKWARD):(HopDirection::FORWARD);
            possibleFullHops.push_back(PossibleFullHop{fullLinkerExtremity.p_fullLinker,
                                                       fullLinkerExtremity.terminus,
                                                       SiteLocation{m_type, originLocation.position-1},
                                                       direction,
                                                       oldAndNewStretch.first,
                                                       oldAndNewStretch.second});
        }
    }
    if(originLocation.position != (m_nSites-1) && m_sites.at(originLocation.position+1).isFree())
    {
        std::pair<double,double> oldAndNewStretch = getOldAndNewStretchFullHop(originLocation.position, originLocation.position+1, positionOppositeExtremity, maxStretch);

        if (std::abs(oldAndNewStretch.second) < maxStretch)
        {
            HopDirection direction = (m_type==MicrotubuleType::FIXED)?(HopDirection::FORWARD):(HopDirection::BACKWARD);
            possibleFullHops.push_back(PossibleFullHop{fullLinkerExtremity.p_fullLinker,
                                                       fullLinkerExtremity.terminus,
                                                       SiteLocation{m_type, originLocation.position+1},
                                                       direction,
                                                       oldAndNewStretch.first,
                                                       oldAndNewStretch.second});
        }
    }
}

// Checks if a possible full connection of possibility.p_fullLinker to possibility.location would not cross any existing fully connected linkers.
// Needs to be called on the microtubule opposite to possibility.p_fullLinker, since the full linker crossing the potential
// Assume that all possibleConnections belong to the same partial linker!
void Microtubule::cleanPossibleCrossings(std::vector<PossibleFullConnection>& possibleConnectionsToCheck, const double mobilePosition, const double maxStretch) const
{
    if(!possibleConnectionsToCheck.empty())
    {
        #ifdef MYDEBUG
        if(possibleConnectionsToCheck.front().location.microtubule!=m_type)
        {
            throw GeneralException("Microtubule::cleanPossibleCrossings() was called on the wrong microtubule");
        }
        #endif // MYDEBUG

        // Store the partial linker and its position now, since there is assumed to be only one partial linker
        Crosslinker*const p_thisPartialLinker = possibleConnectionsToCheck.front().p_partialLinker;

        #ifdef MYDEBUG
        if(!p_thisPartialLinker->isPartial())
        {
            throw GeneralException("Microtubule::cleanPossibleCrossings() encountered a non-partial linker");
        }
        #endif // MYDEBUG

        const SiteLocation locationPartialLinker = p_thisPartialLinker->getBoundLocationWhenPartiallyConnected();

        #ifdef MYDEBUG
        if (locationPartialLinker.microtubule == m_type)
        {
            throw GeneralException("Microtubule::cleanPossibleCrossings() encountered a linker on this microtubule");
        }
        for (const PossibleFullConnection& possibility : possibleConnectionsToCheck)
        {
            if(possibility.p_partialLinker != p_thisPartialLinker)
            {
                throw GeneralException("possibleConnectionsToCheck contains possibilities from different partial linkers, while Microtubule::cleanPossibleCrossings() assumes only one linker");
            }
        }
        #endif // MYDEBUG

        double positionPartialLinker; // Calculate the position relative to this microtubule, which is opposite to the partial linker
        switch(locationPartialLinker.microtubule)
        {
        case MicrotubuleType::FIXED:
            positionPartialLinker = locationPartialLinker.position*m_latticeSpacing - mobilePosition;
            break;
        case MicrotubuleType::MOBILE:
            positionPartialLinker = locationPartialLinker.position*m_latticeSpacing + mobilePosition;
            break;
        default:
            throw GeneralException("switch statement in Microtubule::cleanPossibleCrossings() fell through");
        }

        std::vector<FullConnectionLocations> fullConnections = getFullCrosslinkersCloseTo(positionPartialLinker, maxStretch);

        #ifdef MYDEBUG
        for(const FullConnectionLocations& fullConnection : fullConnections)
        {
            if(fullConnection.locationNextToPartial.microtubule != locationPartialLinker.microtubule || fullConnection.locationOppositeToPartial.microtubule == locationPartialLinker.microtubule)
            {
                throw GeneralException("Microtubule::cleanPossibleCrossings() encountered something fishy in fullConnections");
            }
        }
        #endif // MYDEBUG

        // erase-remove idiom erasing all possibleConnections from possibleConnectionsToCheck that have a full linker in the surroundings that crosses the possibleConnection
        possibleConnectionsToCheck.erase(std::remove_if(possibleConnectionsToCheck.begin(),possibleConnectionsToCheck.end(),
                                                // lambda expression
                                                [&fullConnections, &locationPartialLinker](const PossibleFullConnection& possibleConnection)
                                                {
                                                    for(const FullConnectionLocations& fullConnection : fullConnections)
                                                    {
                                                        // Compare the integers labeling the sites on each microtubule with each other
                                                        if ((locationPartialLinker.position < fullConnection.locationNextToPartial.position) &&
                                                            (possibleConnection.location.position > fullConnection.locationOppositeToPartial.position))
                                                        {
                                                            return true;
                                                        }
                                                        else if((locationPartialLinker.position > fullConnection.locationNextToPartial.position) &&
                                                                (possibleConnection.location.position < fullConnection.locationOppositeToPartial.position))
                                                        {
                                                            return true;
                                                        }
                                                    }
                                                    return false;
                                                }), possibleConnectionsToCheck.end());
    }
}

// position relative to this microtubule. Should return knowing that a partial linker is opposite to this microtubule.
std::vector<FullConnectionLocations> Microtubule::getFullCrosslinkersCloseTo(const double position, const double maxStretch) const
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
        std::vector<FullConnectionLocations> fullsCloseby;
        for (int32_t posToCheck = lowerSiteLabel; posToCheck<=upperSiteLabel; ++posToCheck)
        {
            if(( m_sites.at(posToCheck).isFull()) )
            {
                Crosslinker*const p_fullLinker = m_sites.at(posToCheck).whichCrosslinkerIsBound();
                SiteLocation headLocation = p_fullLinker->getOneBoundLocationWhenFullyConnected(Crosslinker::Terminus::HEAD);
                SiteLocation tailLocation = p_fullLinker->getOneBoundLocationWhenFullyConnected(Crosslinker::Terminus::TAIL);

                // this function has been called on microtubule opposite of a partial linker. Hence, m_type gives the type of the opposite microtubule relative to the partial
                // definition: struct FullConnectionLocations{Crosslinker* p_fullLinker; SiteLocation locationNextToPartial; SiteLocation locationOppositeToPartial;};
                // we (m_type) are opposite to a partial linker, and we are finding the full connections close to that partial
                if(headLocation.microtubule==m_type)
                {
                    fullsCloseby.push_back(FullConnectionLocations{p_fullLinker, tailLocation, headLocation});
                }
                else
                {
                    fullsCloseby.push_back(FullConnectionLocations{p_fullLinker, headLocation, tailLocation});
                }
            }
        }
        return fullsCloseby;
    }
}

// position relative to this microtubule. Only finds the partials of type typeToCheck on this microtubule.
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

// Finds the nearest neighbour partial linkers (relative to originLocation) of type typeToCheck.
std::vector<Crosslinker*> Microtubule::getNeighbouringPartialCrosslinkersOf(const SiteLocation& originLocation, const Crosslinker::Type typeToCheck) const
{
    #ifdef MYDEBUG
    if (originLocation.microtubule != m_type)
    {
        throw GeneralException("Microtubule::getNeighbouringPartialCrosslinkersOf() was called on the wrong microtubule.");
    }
    #endif // MYDEBUG

    std::vector<Crosslinker*> partialNeighbours;

    if((originLocation.position!=0) && (m_sites.at(originLocation.position-1).isPartial())
                                    && (m_sites.at(originLocation.position-1).whichCrosslinkerIsBound()->getType()==typeToCheck))
    {
        partialNeighbours.push_back(m_sites.at(originLocation.position-1).whichCrosslinkerIsBound());
    }

    //
    if((originLocation.position!=(m_nSites-1)) && (m_sites.at(originLocation.position+1).isPartial())
                                               && (m_sites.at(originLocation.position+1).whichCrosslinkerIsBound()->getType()==typeToCheck))
    {
        partialNeighbours.push_back(m_sites.at(originLocation.position+1).whichCrosslinkerIsBound());
    }

    return partialNeighbours;
}

// Finds the nearest neighbour (relative to originLocation) full linker extremities of type typeToCheck.
std::vector<FullExtremity> Microtubule::getNeighbouringFullCrosslinkersOf(const SiteLocation& originLocation, const Crosslinker::Type typeToCheck) const
{
    #ifdef MYDEBUG
    if (originLocation.microtubule != m_type)
    {
        throw GeneralException("Microtubule::getNeighbouringFullCrosslinkersOf() was called on the wrong microtubule.");
    }
    #endif // MYDEBUG

    std::vector<FullExtremity> fullNeighbours;

    if((originLocation.position!=0) && (m_sites.at(originLocation.position-1).isFull())
                                    && (m_sites.at(originLocation.position-1).whichCrosslinkerIsBound()->getType()==typeToCheck))
    {
        Crosslinker*const p_linker = m_sites.at(originLocation.position-1).whichCrosslinkerIsBound();
        fullNeighbours.push_back(FullExtremity{p_linker, p_linker->getTerminusOfFullOn(m_type)});
    }

    if((originLocation.position!=(m_nSites-1)) && (m_sites.at(originLocation.position+1).isFull())
                                    && (m_sites.at(originLocation.position+1).whichCrosslinkerIsBound()->getType()==typeToCheck))
    {
        Crosslinker*const p_linker = m_sites.at(originLocation.position+1).whichCrosslinkerIsBound();
        fullNeighbours.push_back(FullExtremity{p_linker, p_linker->getTerminusOfFullOn(m_type)});
    }

    return fullNeighbours;
}


