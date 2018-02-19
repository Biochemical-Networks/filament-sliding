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

CrosslinkerContainer::CrosslinkerContainer(const int32_t nCrosslinkers, const Crosslinker& defaultCrosslinker, const Crosslinker::Type linkerType)
    :   m_linkerType(linkerType),
        m_crosslinkers(nCrosslinkers, defaultCrosslinker)
{
    // Fill the freeCrosslinkers deque with pointers to all crosslinkers: these are initially free
    for (int32_t i=0; i<nCrosslinkers; ++i)
    {
        m_freeCrosslinkers.push_back(&(m_crosslinkers.at(i)));
    }
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
        throw GeneralException("CrosslinkerContainer.at() went out of bounds.");
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

int32_t CrosslinkerContainer::getNSitesToBindPartial(const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing) const
{
    const double mobilePosition = mobileMicrotubule.getPosition(); // Won't change during the subsequent for-loop

    int32_t nSitesToBindPartial = 0;

    // If there are no partially connected crosslinkers, the for body will not execute, which is how it should be
    for(auto p_linker : m_partialCrosslinkers)
    {
        SiteLocation locationConnectedTo = p_linker->getBoundLocationWhenPartiallyConnected();

        // Check the free sites on the opposite microtubule!
        switch(locationConnectedTo.microtubule)
        {
        case MicrotubuleType::FIXED:
            nSitesToBindPartial += mobileMicrotubule.getNFreeSitesCloseTo(locationConnectedTo.position*latticeSpacing - mobilePosition, maxStretch);
            break;
        case MicrotubuleType::MOBILE:
            nSitesToBindPartial += fixedMicrotubule.getNFreeSitesCloseTo(locationConnectedTo.position*latticeSpacing + mobilePosition, maxStretch);
            break;
        default:
            throw GeneralException("Wrong location stored and encountered in getNSitesToBindPartial()");
        }
    }
    return nSitesToBindPartial;
}

void CrosslinkerContainer::findPossibleConnections(const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch, const double latticeSpacing)
{
    // Empty the container, the following will recalculate the whole vector
    // The capacity of the vector does not change (?)
    m_possibleConnections.clear();

    // If there are no partially connected crosslinkers, the for body will not execute, which is how it should be
    for(Crosslinker* p_linker : m_partialCrosslinkers)
    {
        addPossibleConnections(p_linker, fixedMicrotubule, mobileMicrotubule, maxStretch, latticeSpacing);
    }
}

/* The following function adds all possible connections of *p_newPartialCrosslinker to m_possibleConnections.
 * It does not finish changing m_possibleConnections:
 * it is possible that the new partial linker also occupies the previously free position of a partial linker on the opposite microtubule.
 */
void CrosslinkerContainer::addPossibleConnections(Crosslinker*const p_newPartialCrosslinker,
                                                  const Microtubule& fixedMicrotubule,
                                                  const MobileMicrotubule& mobileMicrotubule,
                                                  const double maxStretch,
                                                  const double latticeSpacing)
{
    SiteLocation locationConnectedTo = p_newPartialCrosslinker->getBoundLocationWhenPartiallyConnected();
    // Check the free sites on the opposite microtubule!
    // Microtubule.getFreeSitesCloseTo changes possibleConnections through a reference, such that no extra vectors need to be made
    switch(locationConnectedTo.microtubule)
    {
    case MicrotubuleType::FIXED:
        mobileMicrotubule.addPossibleConnectionsCloseTo(m_possibleConnections, p_newPartialCrosslinker, locationConnectedTo.position*latticeSpacing - mobileMicrotubule.getPosition(), maxStretch);
        break;
    case MicrotubuleType::MOBILE:
        fixedMicrotubule.addPossibleConnectionsCloseTo(m_possibleConnections, p_newPartialCrosslinker, locationConnectedTo.position*latticeSpacing + mobileMicrotubule.getPosition(), maxStretch);
        break;
    default:
        throw GeneralException("Wrong location stored and encountered in addPossibleConnections()");
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
                                                    if(possibleConnection.p_partialLinker==p_oldPartialCrosslinker)
                                                    {
                                                        return true;
                                                    }
                                                    else
                                                    {
                                                        return false;
                                                    }
                                                }), m_possibleConnections.end());

}


void CrosslinkerContainer::updateConnectionDataFreeToPartial(Crosslinker*const p_newPartialCrosslinker,
                                                                  const Microtubule& fixedMicrotubule,
                                                                  const MobileMicrotubule& mobileMicrotubule,
                                                                  const double maxStretch,
                                                                  const double latticeSpacing)
{
    if(!(p_newPartialCrosslinker->isPartial()))
    {
        throw GeneralException("updateConnectionDataFreeToPartial was called on a crosslinker that is not partial.");
    }
    /* Two types of connections are changed:
     * connections from the new partial crosslinker to the opposite microtubule are added,
     * and old possible connections to the free site are removed
     */
    if(p_newPartialCrosslinker->getType()==m_linkerType)
    {
        addPossibleConnections(p_newPartialCrosslinker, fixedMicrotubule, mobileMicrotubule, maxStretch, latticeSpacing);
    }

    SiteLocation locationConnectedTo = p_newPartialCrosslinker->getBoundLocationWhenPartiallyConnected();

    updatePossibleConnectionsOppositeTo(p_newPartialCrosslinker, locationConnectedTo, fixedMicrotubule, mobileMicrotubule, maxStretch, latticeSpacing);
}

void CrosslinkerContainer::updateConnectionDataPartialToFree(Crosslinker*const p_oldPartialCrosslinker,
                                                                 const SiteLocation locationOldConnection,
                                                                 const Microtubule& fixedMicrotubule,
                                                                 const MobileMicrotubule& mobileMicrotubule,
                                                                 const double maxStretch,
                                                                 const double latticeSpacing)
{
    if(p_oldPartialCrosslinker->isConnected())
    {
        throw GeneralException("updateConnectionDataPartialToFree was called on a crosslinker that is not free.");
    }

    /* Two types of connections are changed:
     * connections from the old partial crosslinker to the opposite microtubule are removed,
     * and possible connections to the new free site are added
     */

    if(p_oldPartialCrosslinker->getType() == m_linkerType)
    {
        removePossibleConnections(p_oldPartialCrosslinker);
    }

    updatePossibleConnectionsOppositeTo(p_oldPartialCrosslinker, locationOldConnection, fixedMicrotubule, mobileMicrotubule, maxStretch, latticeSpacing);
}

void CrosslinkerContainer::updateConnectionDataFullToPartial(Crosslinker*const p_newPartialCrosslinker,
                                                                  const SiteLocation locationOldConnection,
                                                                  const Microtubule& fixedMicrotubule,
                                                                  const MobileMicrotubule& mobileMicrotubule,
                                                                  const double maxStretch,
                                                                  const double latticeSpacing)
{
    if(!(p_newPartialCrosslinker->isPartial()))
    {
        throw GeneralException("updateConnectionDataFullToPartial was called on a crosslinker that is not partial.");
    }
    /* Two types of connections are changed:
     * connections from the new partial crosslinker to the opposite microtubule are added,
     * and possible connections to the newly freed site are added
     */
    if(p_newPartialCrosslinker->getType()==m_linkerType)
    {
        addPossibleConnections(p_newPartialCrosslinker, fixedMicrotubule, mobileMicrotubule, maxStretch, latticeSpacing);
    }

    updatePossibleConnectionsOppositeTo(p_newPartialCrosslinker, locationOldConnection, fixedMicrotubule, mobileMicrotubule, maxStretch, latticeSpacing);

    // Update m_fullConnections:
    removeFullConnection(p_newPartialCrosslinker);
}

void CrosslinkerContainer::updateConnectionDataPartialToFull(Crosslinker*const p_oldPartialCrosslinker,
                                                                  const SiteLocation locationNewConnection,
                                                                  const Microtubule& fixedMicrotubule,
                                                                  const MobileMicrotubule& mobileMicrotubule,
                                                                  const double maxStretch,
                                                                  const double latticeSpacing)
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
    }

    updatePossibleConnectionsOppositeTo(p_oldPartialCrosslinker, locationNewConnection, fixedMicrotubule, mobileMicrotubule, maxStretch, latticeSpacing);

    // Update m_fullConnections:
    addFullConnection(p_oldPartialCrosslinker, mobileMicrotubule.getPosition(), latticeSpacing, maxStretch);
}

/* This function updates the possible connections for partials close to locationConnection, on the opposite microtubule.
 * One (ex-)partial crosslinker is ignored, *p_partialCrosslinker. This usually represents the crosslinker that was just removed or added.
 */
void CrosslinkerContainer::updatePossibleConnectionsOppositeTo(Crosslinker*const p_partialCrosslinker,
                                                                const SiteLocation locationConnection,
                                                                const Microtubule& fixedMicrotubule,
                                                                const MobileMicrotubule& mobileMicrotubule,
                                                                const double maxStretch,
                                                                const double latticeSpacing)
{
    std::vector<Crosslinker*> partialNeighbours;

    double positionRelativeToOppositeMicrotubule;

    switch(locationConnection.microtubule)
    {
    case MicrotubuleType::FIXED:
        positionRelativeToOppositeMicrotubule = locationConnection.position*latticeSpacing - mobileMicrotubule.getPosition();
        partialNeighbours = mobileMicrotubule.getPartialCrosslinkersCloseTo(positionRelativeToOppositeMicrotubule, maxStretch, m_linkerType);
        break;
    case MicrotubuleType::MOBILE:
        positionRelativeToOppositeMicrotubule = locationConnection.position*latticeSpacing + mobileMicrotubule.getPosition();
        partialNeighbours = fixedMicrotubule.getPartialCrosslinkersCloseTo(positionRelativeToOppositeMicrotubule, maxStretch, m_linkerType);
        break;
    default:
        throw GeneralException("Wrong location stored and encountered in updatePossibleConnectionsOppositeTo()");
    }

    for (Crosslinker* p_linker : partialNeighbours)
    {
        // p_partialCrosslinker can be one of the linkers, but then this one should have been updated already
        if (p_linker!=p_partialCrosslinker)
        {
            // First remove the connections involving the linker, and then add them again, where the possibly newly occupied site is taken into account
            // The linkers are guaranteed to be of the correct type
            removePossibleConnections(p_linker);
            addPossibleConnections(p_linker, fixedMicrotubule, mobileMicrotubule, maxStretch, latticeSpacing);
        }
    }
}

bool CrosslinkerContainer::fullLinkersAllowMobilePositionChange(const double positionChange, const double maxStretch) const
{
    for (const FullConnection connection : m_fullConnections)
    {
        if(std::abs(connection.extension + positionChange) >= maxStretch)
        {
            return false;
        }
    }
    return true;
}

bool CrosslinkerContainer::partialPossibleConnectionsConformToMobilePositionChange(const double positionChange, const double maxStretch) const
{
    for (const PossibleFullConnection connection : m_possibleConnections)
    {
        if(std::abs(connection.extension + positionChange) >= maxStretch)
        {
            return false;
        }
    }
    return true;
}

void CrosslinkerContainer::addFullConnection(Crosslinker*const p_newFullCrosslinker, const double mobilePosition, const double latticeSpacing, const double maxStretch)
{
    SiteLocation headLocation = p_newFullCrosslinker->getOneBoundLocationWhenFullyConnected(Crosslinker::Terminus::HEAD);
    SiteLocation tailLocation = p_newFullCrosslinker->getOneBoundLocationWhenFullyConnected(Crosslinker::Terminus::TAIL);

    double extension;

    switch(headLocation.microtubule)
    {
    case MicrotubuleType::FIXED:
        extension = mobilePosition + latticeSpacing*(tailLocation.position - headLocation.position);
        break;
    case MicrotubuleType::MOBILE:
        extension = mobilePosition + latticeSpacing*(headLocation.position - tailLocation.position);
        break;
    default:
        throw GeneralException("A wrong microtubule type encountered in addFullConnection()");
    }

    if(std::abs(extension)>=maxStretch)
    {
        throw GeneralException("addFullConnection() tried to add a full connection with a disallowed stretch.");
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
                                                    if(fullConnection.p_fullLinker==p_oldFullCrosslinker)
                                                    {
                                                        return true;
                                                    }
                                                    else
                                                    {
                                                        return false;
                                                    }
                                                }), m_fullConnections.end());

}


#ifdef MYDEBUG
Crosslinker* CrosslinkerContainer::TESTgetAFullCrosslinker(const int32_t which) const
{
    return m_fullCrosslinkers.at(which);
}
#endif //MYDEBUG
