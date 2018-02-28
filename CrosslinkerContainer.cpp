#include "CrosslinkerContainer.hpp"

#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"
#include "MicrotubuleType.hpp"
#include "PossibleFullConnection.hpp"
#include "Microtubule.hpp"
#include "MobileMicrotubule.hpp"

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <limits>

CrosslinkerContainer::CrosslinkerContainer(const int32_t nCrosslinkers,
                                           const Crosslinker& defaultCrosslinker,
                                           const Crosslinker::Type linkerType,
                                           const Microtubule& fixedMicrotubule,
                                           const MobileMicrotubule& mobileMicrotubule,
                                           const double latticeSpacing,
                                           const double maxStretch)
    :   m_linkerType(linkerType),
        m_crosslinkers(nCrosslinkers, defaultCrosslinker),
        m_fixedMicrotubule(fixedMicrotubule),
        m_mobileMicrotubule(mobileMicrotubule),
        m_latticeSpacing(latticeSpacing),
        m_maxStretch(maxStretch),
        m_mod1(myMod(m_maxStretch, m_latticeSpacing)),
        m_mod2(myMod(-m_maxStretch, m_latticeSpacing))
{
    // Fill the freeCrosslinkers vector with pointers to all crosslinkers: these are initially free
    for (int32_t i=0; i<nCrosslinkers; ++i)
    {
        m_freeCrosslinkers.push_back(&(m_crosslinkers.at(i)));
    }

    if(!std::numeric_limits<double>::is_iec559)
    {
        throw GeneralException("An algorithm in the class CrosslinkerContainer uses the IEEE 754 standard to represent positive and negative infinity");
    }

    // Borders for the region in which possibilities are valid need to be initialised. Non-connected microtubules have these imaginary borders, and they are only updated after they exist.
    // However, the mobile needs to have a position first. Therefore, initialisation of the borders happens upon setting an initial position through SystemState.
}

CrosslinkerContainer::~CrosslinkerContainer()
{
}

Crosslinker& CrosslinkerContainer::at(const int32_t position)
{
    // Convert the std::out_of_range error into a GeneralException, such that a text is printed when it goes wrong.
    //Also, it is possible for the caller to catch it, without having to know what kind of error happened.
    try
    {
        return m_crosslinkers.at(position);
    }
    catch(std::out_of_range) // For the at function
    {
        throw GeneralException("CrosslinkerContainer::at() went out of bounds.");
    }
}

Crosslinker* CrosslinkerContainer::connectFromFreeToPartial()
{
    // All free crosslinkers are in the same state, so it doesn't matter which one is taken. Therefore, we take the final one in the row.
    Crosslinker* p_crosslinkerToConnect = m_freeCrosslinkers.back(); // Returns last element, which is a pointer
    m_freeCrosslinkers.pop_back();
    m_partialCrosslinkers.push_back(p_crosslinkerToConnect);

    return p_crosslinkerToConnect;
}

void CrosslinkerContainer::disconnectFromPartialToFree(Crosslinker& crosslinkerToDisconnect)
{
    m_partialCrosslinkers.erase(std::remove(m_partialCrosslinkers.begin(), m_partialCrosslinkers.end(), &crosslinkerToDisconnect));
    m_freeCrosslinkers.push_back(&crosslinkerToDisconnect);
}

void CrosslinkerContainer::connectFromPartialToFull(Crosslinker& crosslinkerToConnect)
{
    m_partialCrosslinkers.erase(std::remove(m_partialCrosslinkers.begin(), m_partialCrosslinkers.end(), &crosslinkerToConnect));
    m_fullCrosslinkers.push_back(&crosslinkerToConnect);
}

void CrosslinkerContainer::disconnectFromFullToPartial(Crosslinker& crosslinkerToDisconnect)
{
    m_fullCrosslinkers.erase(std::remove(m_fullCrosslinkers.begin(), m_fullCrosslinkers.end(), &crosslinkerToDisconnect));
    m_partialCrosslinkers.push_back(&crosslinkerToDisconnect);
}

int32_t CrosslinkerContainer::getNCrosslinkers() const
{
    return m_crosslinkers.size();
}

int32_t CrosslinkerContainer::getNFreeCrosslinkers() const
{
    return m_freeCrosslinkers.size();
}

int32_t CrosslinkerContainer::getNPartialCrosslinkers() const
{
    return m_partialCrosslinkers.size();
}

int32_t CrosslinkerContainer::getNFullCrosslinkers() const
{
    return m_fullCrosslinkers.size();
}

#ifdef MYDEBUG
int32_t CrosslinkerContainer::getNSitesToBindPartial() const
{
    const double mobilePosition = m_mobileMicrotubule.getPosition(); // Won't change during the subsequent for-loop

    int32_t nSitesToBindPartial = 0;

    // If there are no partially connected crosslinkers, the for body will not execute, which is how it should be
    for(Crosslinker*const p_linker : m_partialCrosslinkers)
    {
        SiteLocation locationConnectedTo = p_linker->getBoundLocationWhenPartiallyConnected();

        // Check the free sites on the opposite microtubule!
        switch(locationConnectedTo.microtubule)
        {
        case MicrotubuleType::FIXED:
            nSitesToBindPartial += m_mobileMicrotubule.getNFreeSitesCloseTo(locationConnectedTo.position*m_latticeSpacing - mobilePosition, m_maxStretch);
            break;
        case MicrotubuleType::MOBILE:
            nSitesToBindPartial += m_fixedMicrotubule.getNFreeSitesCloseTo(locationConnectedTo.position*m_latticeSpacing + mobilePosition, m_maxStretch);
            break;
        default:
            throw GeneralException("Wrong location stored and encountered in CrosslinkerContainer::getNSitesToBindPartial()");
        }
    }
    return nSitesToBindPartial;
}
#endif // MYDEBUG

void CrosslinkerContainer::findPossibleConnections()
{
    // Empty the container, the following will recalculate the whole vector
    // The capacity of the vector does not change (not defined by standard)
    m_possibleConnections.clear();

    // If there are no partially connected crosslinkers, the for body will not execute, which is how it should be
    for(Crosslinker*const p_linker : m_partialCrosslinkers)
    {
        addPossibleConnections(p_linker);
    }
}

void CrosslinkerContainer::findPossiblePartialHops()
{
    // Empty the container, the following will recalculate the whole vector
    // The capacity of the vector does probably not change (not defined by standard)
    m_possiblePartialHops.clear();

    // If there are no partially connected crosslinkers, the for body will not execute, which is how it should be
    for(Crosslinker*const p_linker : m_partialCrosslinkers)
    {
        addPossiblePartialHops(p_linker);
    }
}

void CrosslinkerContainer::findPossibleFullHops()
{
    // Empty the container, the following will recalculate the whole vector
    // The capacity of the vector does probably not change (not defined by standard)
    m_possibleFullHops.clear();

    // If there are no partially connected crosslinkers, the for body will not execute, which is how it should be
    for(Crosslinker*const p_linker : m_fullCrosslinkers)
    {
        addPossibleFullHops(p_linker); // Adds the possible hops for both extremities of the full linker
    }
}

/* The following function adds all possible connections of *p_newPartialCrosslinker to m_possibleConnections.
 * It does not finish changing m_possibleConnections:
 * it is possible that the new partial linker also occupies the previously free position of a partial linker on the opposite microtubule.
 */
void CrosslinkerContainer::addPossibleConnections(Crosslinker*const p_newPartialCrosslinker)
{
    SiteLocation locationConnectedTo = p_newPartialCrosslinker->getBoundLocationWhenPartiallyConnected();
    // Check the free sites on the opposite microtubule!
    // Microtubule.getFreeSitesCloseTo changes possibleConnections through a reference, such that no extra vectors need to be made
    switch(locationConnectedTo.microtubule)
    {
    case MicrotubuleType::FIXED:
        m_mobileMicrotubule.addPossibleConnectionsCloseTo(m_possibleConnections, p_newPartialCrosslinker,
                                                          locationConnectedTo.position*m_latticeSpacing - m_mobileMicrotubule.getPosition(), m_mobileMicrotubule.getPosition(), m_maxStretch);
        break;
    case MicrotubuleType::MOBILE:
        m_fixedMicrotubule.addPossibleConnectionsCloseTo(m_possibleConnections, p_newPartialCrosslinker,
                                                         locationConnectedTo.position*m_latticeSpacing + m_mobileMicrotubule.getPosition(), m_mobileMicrotubule.getPosition(), m_maxStretch);
        break;
    default:
        throw GeneralException("Wrong location stored and encountered in CrosslinkerContainer::addPossibleConnections()");
    }
}

void CrosslinkerContainer::removePossibleConnections(Crosslinker*const p_oldPartialCrosslinker)
{
    // Use a lambda expression as a predicate for std::remove_if
    // erase-remove idiom erases all elements complying to the predicate
    m_possibleConnections.erase(std::remove_if(m_possibleConnections.begin(),m_possibleConnections.end(),
                                                // lambda expression, capturing p_oldPartialCrosslinker by value, since it is a pointer
                                                [p_oldPartialCrosslinker](const PossibleFullConnection& possibleConnection)
                                                {
                                                    // The identity of a partial linker is checked through its pointer (memory location)
                                                    // This is okay as long as the CrosslinkerContainer class guarantees that m_crosslinkers is never resized.
                                                    return possibleConnection.p_partialLinker==p_oldPartialCrosslinker;
                                                }), m_possibleConnections.end());

}

void CrosslinkerContainer::addPossiblePartialHops(Crosslinker*const p_newPartialCrosslinker)
{
    SiteLocation locationConnectedTo = p_newPartialCrosslinker->getBoundLocationWhenPartiallyConnected();

    switch(locationConnectedTo.microtubule)
    {
    case MicrotubuleType::FIXED:
        m_fixedMicrotubule.addPossiblePartialHopsCloseTo(m_possiblePartialHops, p_newPartialCrosslinker);
        break;
    case MicrotubuleType::MOBILE:
        m_mobileMicrotubule.addPossiblePartialHopsCloseTo(m_possiblePartialHops, p_newPartialCrosslinker);
        break;
    default:
        throw GeneralException("Wrong location stored and encountered in CrosslinkerContainer::addPossiblePartialHops()");
    }
}

void CrosslinkerContainer::removePossiblePartialHops(Crosslinker*const p_oldPartialCrosslinker)
{
    // Use a lambda expression as a predicate for std::remove_if
    // erase-remove idiom erases all elements complying to the predicate
    m_possiblePartialHops.erase(std::remove_if(m_possiblePartialHops.begin(),m_possiblePartialHops.end(),
                                                // lambda expression, capturing p_oldPartialCrosslinker by value, since it is a pointer
                                                [p_oldPartialCrosslinker](const PossiblePartialHop& possiblePartialHop)
                                                {
                                                    // The identity of a partial linker is checked through its pointer (memory location)
                                                    // This is okay as long as the CrosslinkerContainer class guarantees that m_crosslinkers is never resized.
                                                    return possiblePartialHop.p_partialLinker==p_oldPartialCrosslinker;
                                                }), m_possiblePartialHops.end());
}

void CrosslinkerContainer::addPossibleFullHops(Crosslinker*const p_newFullCrosslinker)
{
    SiteLocation headLocation = p_newFullCrosslinker->getSiteLocationOf(Crosslinker::Terminus::HEAD);
    SiteLocation tailLocation = p_newFullCrosslinker->getSiteLocationOf(Crosslinker::Terminus::TAIL);

    #ifdef MYDEBUG
    if (headLocation.microtubule == tailLocation.microtubule)
    {
        throw GeneralException("CrosslinkerContainer::addPossibleFullHops() encountered a full linker that was doubly connected to a single microtubule.");
    }
    #endif // MYDEBUG

    switch(headLocation.microtubule)
    {
    case MicrotubuleType::FIXED:
        m_fixedMicrotubule.addPossibleFullHopsCloseTo(m_possibleFullHops, FullExtremity{p_newFullCrosslinker, Crosslinker::Terminus::HEAD},
                                                      tailLocation.position*m_latticeSpacing + m_mobileMicrotubule.getPosition(), m_maxStretch);
        m_mobileMicrotubule.addPossibleFullHopsCloseTo(m_possibleFullHops, FullExtremity{p_newFullCrosslinker, Crosslinker::Terminus::TAIL},
                                                      headLocation.position*m_latticeSpacing - m_mobileMicrotubule.getPosition(), m_maxStretch);
        break;
    case MicrotubuleType::MOBILE:
        m_mobileMicrotubule.addPossibleFullHopsCloseTo(m_possibleFullHops, FullExtremity{p_newFullCrosslinker, Crosslinker::Terminus::HEAD},
                                                       tailLocation.position*m_latticeSpacing - m_mobileMicrotubule.getPosition(), m_maxStretch);
        m_fixedMicrotubule.addPossibleFullHopsCloseTo(m_possibleFullHops, FullExtremity{p_newFullCrosslinker, Crosslinker::Terminus::TAIL},
                                                       headLocation.position*m_latticeSpacing + m_mobileMicrotubule.getPosition(), m_maxStretch);
        break;
    default:
        throw GeneralException("Wrong location stored and encountered in CrosslinkerContainer::addPossiblePartialHops()");
    }
}

void CrosslinkerContainer::removePossibleFullHops(Crosslinker*const p_oldFullCrosslinker)
{
    // Use a lambda expression as a predicate for std::remove_if
    // erase-remove idiom erases all elements complying to the predicate
    m_possibleFullHops.erase(std::remove_if(m_possibleFullHops.begin(),m_possibleFullHops.end(),
                                                // lambda expression, capturing p_oldPartialCrosslinker by value, since it is a pointer
                                                [p_oldFullCrosslinker](const PossibleFullHop& possibleFullHop)
                                                {
                                                    // The identity of a full linker is checked through its pointer (memory location)
                                                    // This is okay as long as the CrosslinkerContainer class guarantees that m_crosslinkers is never resized.
                                                    return possibleFullHop.p_fullLinker==p_oldFullCrosslinker;
                                                }), m_possibleFullHops.end());
}

void CrosslinkerContainer::updateConnectionDataFreeToPartial(Crosslinker*const p_newPartialCrosslinker)
{
    if(!(p_newPartialCrosslinker->isPartial()))
    {
        throw GeneralException("CrosslinkerContainer::updateConnectionDataFreeToPartial() was called on a crosslinker that is not partial.");
    }
    /* Two types of connections are changed:
     * connections from the new partial crosslinker to the opposite microtubule are added,
     * and old possible connections to the free site are removed
     */
    if(p_newPartialCrosslinker->getType()==m_linkerType)
    {
        addPossibleConnections(p_newPartialCrosslinker);

        addPossiblePartialHops(p_newPartialCrosslinker);
    }

    SiteLocation locationConnectedTo = p_newPartialCrosslinker->getBoundLocationWhenPartiallyConnected();

    updatePossibleConnectionsOppositeTo(p_newPartialCrosslinker, locationConnectedTo);

    updatePossiblePartialHopsNextTo(locationConnectedTo);

    updatePossibleFullHopsNextTo(locationConnectedTo);
}

void CrosslinkerContainer::updateConnectionDataPartialToFree(Crosslinker*const p_oldPartialCrosslinker, const SiteLocation locationOldConnection)
{
    if(p_oldPartialCrosslinker->isConnected())
    {
        throw GeneralException("CrosslinkerContainer::updateConnectionDataPartialToFree() was called on a crosslinker that is not free.");
    }

    /* Two types of connections are changed:
     * connections from the old partial crosslinker to the opposite microtubule are removed,
     * and possible connections to the new free site are added
     */

    if(p_oldPartialCrosslinker->getType() == m_linkerType)
    {
        removePossibleConnections(p_oldPartialCrosslinker);

        removePossiblePartialHops(p_oldPartialCrosslinker);
    }

    updatePossibleConnectionsOppositeTo(p_oldPartialCrosslinker, locationOldConnection);

    updatePossiblePartialHopsNextTo(locationOldConnection);

    updatePossibleFullHopsNextTo(locationOldConnection);
}

void CrosslinkerContainer::updateConnectionDataFullToPartial(Crosslinker*const p_newPartialCrosslinker, const SiteLocation locationOldConnection)
{
    if(!(p_newPartialCrosslinker->isPartial()))
    {
        throw GeneralException("CrosslinkerContainer::updateConnectionDataFullToPartial() was called on a crosslinker that is not partial.");
    }
    /* Two types of connections are changed:
     * connections from the new partial crosslinker to the opposite microtubule are added,
     * and possible connections to the newly freed site are added
     */
    if(p_newPartialCrosslinker->getType()==m_linkerType)
    {
        addPossibleConnections(p_newPartialCrosslinker);

        addPossiblePartialHops(p_newPartialCrosslinker);

        removePossibleFullHops(p_newPartialCrosslinker);

        // Update m_fullConnections:
        removeFullConnection(p_newPartialCrosslinker);
    }

    updatePossibleConnectionsOppositeTo(p_newPartialCrosslinker, locationOldConnection);

    updatePossiblePartialHopsNextTo(locationOldConnection);

    updatePossibleFullHopsNextTo(locationOldConnection);
}

void CrosslinkerContainer::updateConnectionDataPartialToFull(Crosslinker*const p_oldPartialCrosslinker, const SiteLocation locationNewConnection)
{
    if(!(p_oldPartialCrosslinker->isFull()))
    {
        throw GeneralException("updateConnectionDataPartialToFull was called on a crosslinker that is not full.");
    }

    /* Two types of connections are changed:
     * connections from the old partial crosslinker to the opposite microtubule are removed,
     * and old possible connections to the newly occupied site are removed
     */

    if(p_oldPartialCrosslinker->getType() == m_linkerType)
    {
        removePossibleConnections(p_oldPartialCrosslinker);

        removePossiblePartialHops(p_oldPartialCrosslinker);

        addPossibleFullHops(p_oldPartialCrosslinker);

        // Update m_fullConnections:
        addFullConnection(p_oldPartialCrosslinker);
    }

    updatePossibleConnectionsOppositeTo(p_oldPartialCrosslinker, locationNewConnection);

    updatePossiblePartialHopsNextTo(locationNewConnection);

    updatePossibleFullHopsNextTo(locationNewConnection);
}

/* This function updates the possible connections for partials close to locationConnection, on the opposite microtubule.
 * One (ex-)partial crosslinker is ignored, *p_partialCrosslinker. This usually represents the crosslinker that was just removed or added.
 */
void CrosslinkerContainer::updatePossibleConnectionsOppositeTo(Crosslinker*const p_partialCrosslinker, const SiteLocation locationConnection)
{
    std::vector<Crosslinker*> partialNeighbours;

    double positionRelativeToOppositeMicrotubule;

    switch(locationConnection.microtubule)
    {
    case MicrotubuleType::FIXED:
        positionRelativeToOppositeMicrotubule = locationConnection.position*m_latticeSpacing - m_mobileMicrotubule.getPosition();
        partialNeighbours = m_mobileMicrotubule.getPartialCrosslinkersCloseTo(positionRelativeToOppositeMicrotubule, m_maxStretch, m_linkerType);
        break;
    case MicrotubuleType::MOBILE:
        positionRelativeToOppositeMicrotubule = locationConnection.position*m_latticeSpacing + m_mobileMicrotubule.getPosition();
        partialNeighbours = m_fixedMicrotubule.getPartialCrosslinkersCloseTo(positionRelativeToOppositeMicrotubule, m_maxStretch, m_linkerType);
        break;
    default:
        throw GeneralException("Wrong location stored and encountered in CrosslinkerContainer::updatePossibleConnectionsOppositeTo()");
    }

    for (Crosslinker*const p_linker : partialNeighbours)
    {
        // p_partialCrosslinker can be one of the linkers, but then this one should have been updated already
        if (p_linker!=p_partialCrosslinker)
        {
            // First remove the connections involving the linker, and then add them again, where the possibly newly occupied site is taken into account
            // The linkers are guaranteed to be of the correct type
            removePossibleConnections(p_linker);
            addPossibleConnections(p_linker);
        }
    }
}

void CrosslinkerContainer::updatePossiblePartialHopsNextTo(const SiteLocation& originLocation)
{
    std::vector<Crosslinker*> partialNeighbours;

    switch(originLocation.microtubule)
    {
    case MicrotubuleType::FIXED:
        partialNeighbours = m_fixedMicrotubule.getNeighbouringPartialCrosslinkersOf(originLocation, m_linkerType);
        break;
    case MicrotubuleType::MOBILE:
        partialNeighbours = m_mobileMicrotubule.getNeighbouringPartialCrosslinkersOf(originLocation, m_linkerType);
        break;
    default:
        throw GeneralException("Wrong location stored and encountered in CrosslinkerContainer::updatePossiblePartialHopsNextTo()");
    }

    for (Crosslinker*const p_linker : partialNeighbours)
    {
        removePossiblePartialHops(p_linker);
        addPossiblePartialHops(p_linker);
    }
}

void CrosslinkerContainer::updatePossibleFullHopsNextTo(const SiteLocation& originLocation)
{
    std::vector<FullExtremity> fullNeighbours;

    switch(originLocation.microtubule)
    {
    case MicrotubuleType::FIXED:
        fullNeighbours = m_fixedMicrotubule.getNeighbouringFullCrosslinkersOf(originLocation, m_linkerType);
        break;
    case MicrotubuleType::MOBILE:
        fullNeighbours = m_mobileMicrotubule.getNeighbouringFullCrosslinkersOf(originLocation, m_linkerType);
        break;
    default:
        throw GeneralException("Wrong location stored and encountered in CrosslinkerContainer::updatePossibleFullHopsNextTo()");
    }

    for (const FullExtremity& fullExtremity: fullNeighbours)
    {
        removePossibleFullHops(fullExtremity.p_fullLinker); // removes the possible hops of both extremities, so
        addPossibleFullHops(fullExtremity.p_fullLinker); // this adds the possible hops of both extremities again, in the updated configuration
    }
}

std::pair<double, double> CrosslinkerContainer::movementBordersSetByFullLinkers() const
{
    if(m_fullCrosslinkers.empty())
    {
        return std::pair<double,double>(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
    }
    // First, find the smallest and largest stretch
    // Initialise the borders with the first extension, since this is initially both the smallest and the largest
    double smallestStretch = m_fullConnections.front().extension;
    double largestStretch = smallestStretch;
    for (const FullConnection& connection : m_fullConnections)
    {
        if(connection.extension < smallestStretch)
        {
            smallestStretch = connection.extension;
        }
        else if(connection.extension > largestStretch) // The maximum cannot change if the minimum changes
        {
            largestStretch = connection.extension;
        }
    }

    return std::pair<double,double>((-m_maxStretch-smallestStretch), (m_maxStretch-largestStretch));
}

double CrosslinkerContainer::myMod(const double x, const double y) const
{
    // both std::fmod and std::remainder do not give the results I need, need mod(1.1,1) = 0.1, mod(-0.1, 1) = 0.9
    return x - (std::floor(x/y)*y);
}

void CrosslinkerContainer::findPossibilityBorders()
{
    const double currentPosition = m_mobileMicrotubule.getPosition();

    const double lowerBorder1 = std::floor((currentPosition-m_mod1)/m_latticeSpacing)*m_latticeSpacing + m_mod1;
    const double upperBorder1 = std::ceil((currentPosition-m_mod1)/m_latticeSpacing)*m_latticeSpacing + m_mod1;
    const double lowerBorder2 = std::floor((currentPosition-m_mod2)/m_latticeSpacing)*m_latticeSpacing + m_mod2;
    const double upperBorder2 = std::ceil((currentPosition-m_mod2)/m_latticeSpacing)*m_latticeSpacing + m_mod2;

    m_lowerBorderPossibilities = std::max(lowerBorder1, lowerBorder2);
    m_upperBorderPossibilities = std::min(upperBorder1, upperBorder2);
}

void CrosslinkerContainer::updateConnectionDataMobilePositionChange(const double positionChange)
{
    // This function assumes that the change is possible and has already happened, in the sense that mobileMicrotubule.position has changed
    // Update extension of the full linkers
    for (FullConnection& connection : m_fullConnections)
    {
        connection.extension += positionChange;
    }

    // Calculate the boundaries over which possibilities could change.
    // This is done because possibilities change either through 1) some not possible any more 2) new possibilities. The former can be checked, the latter can only be done through recalculation
    // Hence, we just always recalculate is the change could make new possibilities available.

    double newPosition = m_mobileMicrotubule.getPosition();

    if(newPosition>m_lowerBorderPossibilities && newPosition < m_upperBorderPossibilities)
    {
        for (PossibleFullConnection& connection : m_possibleConnections)
        {
            connection.extension += positionChange;
        }
        for (PossibleFullHop& hop : m_possibleFullHops)
        {
            hop.oldExtension += positionChange;
            hop.newExtension += positionChange;
        }
    }
    else
    {
        // Recalculate all possibilities completely to get rid of outdated ones and include new ones. Also resets the borders
        resetPossibilities();
    }
}

void CrosslinkerContainer::addFullConnection(Crosslinker*const p_newFullCrosslinker)
{
    SiteLocation headLocation = p_newFullCrosslinker->getOneBoundLocationWhenFullyConnected(Crosslinker::Terminus::HEAD);
    SiteLocation tailLocation = p_newFullCrosslinker->getOneBoundLocationWhenFullyConnected(Crosslinker::Terminus::TAIL);

    double extension;

    switch(headLocation.microtubule)
    {
    case MicrotubuleType::FIXED:
        extension = m_mobileMicrotubule.getPosition() + m_latticeSpacing*(tailLocation.position - headLocation.position);
        break;
    case MicrotubuleType::MOBILE:
        extension = m_mobileMicrotubule.getPosition() + m_latticeSpacing*(headLocation.position - tailLocation.position);
        break;
    default:
        throw GeneralException("A wrong microtubule type encountered in CrosslinkerContainer::addFullConnection()");
    }

    if(std::abs(extension)>=m_maxStretch)
    {
        throw GeneralException("CrosslinkerContainer::addFullConnection() tried to add a full connection with a disallowed stretch.");
    }

    m_fullConnections.push_back(FullConnection{p_newFullCrosslinker, extension});

}

void CrosslinkerContainer::removeFullConnection(Crosslinker*const p_oldFullCrosslinker)
{
    // Use a lambda expression as a predicate for std::remove_if
    // erase-remove idiom erases all elements complying to the predicate
    m_fullConnections.erase(std::remove_if(m_fullConnections.begin(),m_fullConnections.end(),
                                                // lambda expression, capturing p_oldFullCrosslinker by value, since it is a pointer
                                                [p_oldFullCrosslinker](const FullConnection& fullConnection)
                                                {
                                                    // The identity of a full linker is checked through its pointer (memory location)
                                                    // This is okay as long as the CrosslinkerContainer class guarantees that m_crosslinkers is never resized.
                                                    return fullConnection.p_fullLinker==p_oldFullCrosslinker;
                                                }), m_fullConnections.end());

}

const std::vector<PossibleFullConnection>& CrosslinkerContainer::getPossibleConnections() const
{
    return m_possibleConnections;
}

const std::vector<FullConnection>& CrosslinkerContainer::getFullConnections() const
{
    return m_fullConnections;
}

const std::vector<Crosslinker*>& CrosslinkerContainer::getPartialLinkers() const
{
    return m_partialCrosslinkers;
}

void CrosslinkerContainer::resetPossibilities()
{
    findPossibilityBorders();

    findPossibleConnections();

    findPossiblePartialHops();

    findPossibleFullHops();
}

#ifdef MYDEBUG
Crosslinker* CrosslinkerContainer::TESTgetAFullCrosslinker(const int32_t which) const
{
    return m_fullCrosslinkers.at(which);
}
#endif // MYDEBUG



