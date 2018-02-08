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

CrosslinkerContainer::CrosslinkerContainer(const int32_t nCrosslinkers, const Crosslinker& defaultCrosslinker)
    :   m_crosslinkers(nCrosslinkers, defaultCrosslinker)
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

int32_t CrosslinkerContainer::getNSitesToBindPartial(const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch) const
{
    const double mobilePosition = mobileMicrotubule.getPosition(); // Won't change during the subsequent for-loop

    int32_t nSitesToBindPartial = 0;

    // If there are no partially connected crosslinkers, the for body will not execute, which is how it should be
    for(auto p_linker : m_partialCrosslinkers)
    {
        SiteLocation locationConnectedTo = p_linker->getBoundPositionWhenPartiallyConnected();

        // Check the free sites on the opposite microtubule!
        switch(locationConnectedTo.microtubule)
        {
        case MicrotubuleType::FIXED:
            nSitesToBindPartial += mobileMicrotubule.getNFreeSitesCloseTo(locationConnectedTo.position - mobilePosition, maxStretch);
            break;
        case MicrotubuleType::MOBILE:
            nSitesToBindPartial += fixedMicrotubule.getNFreeSitesCloseTo(locationConnectedTo.position, maxStretch);
            break;
        default:
            throw GeneralException("Wrong location stored and encountered in getNSitesToBindPartial()");
        }
    }
    return nSitesToBindPartial;
}

void CrosslinkerContainer::findPossibleConnections(const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch)
{
    // Empty the container, the following will recalculate the whole vector
    // The capacity of the vector does not change (?)
    m_possibleConnections.clear();

    // If there are no partially connected crosslinkers, the for body will not execute, which is how it should be
    for(Crosslinker* p_linker : m_partialCrosslinkers)
    {
        addPossibleConnections(p_linker, fixedMicrotubule, mobileMicrotubule, maxStretch);
    }
}

/* The following function adds all possible connections of *p_newPartialCrosslinker to m_possibleConnections.
 * It does not finish changing m_possibleConnections:
 * it is possible that the new partial linker also occupies the previously free position of a partial linker on the opposite microtubule.
 */
void CrosslinkerContainer::addPossibleConnections(Crosslinker*const p_newPartialCrosslinker, const Microtubule& fixedMicrotubule, const MobileMicrotubule& mobileMicrotubule, const double maxStretch)
{
    if(!(p_newPartialCrosslinker->isPartial()))
    {
        throw GeneralException("The Crosslinker pointer passed to CrosslinkerContainer::addPossibleConnections() is not partial yet, which is assumed.");
    }
    SiteLocation locationConnectedTo = p_newPartialCrosslinker->getBoundPositionWhenPartiallyConnected();
    // Check the free sites on the opposite microtubule!
    // Microtubule.getFreeSitesCloseTo changes possibleConnections through a reference, such that no extra vectors need to be made
    switch(locationConnectedTo.microtubule)
    {
    case MicrotubuleType::FIXED:
        mobileMicrotubule.addFreeSitesCloseTo(m_possibleConnections, p_newPartialCrosslinker, locationConnectedTo.position - mobileMicrotubule.getPosition(), maxStretch);
        break;
    case MicrotubuleType::MOBILE:
        fixedMicrotubule.addFreeSitesCloseTo(m_possibleConnections, p_newPartialCrosslinker, locationConnectedTo.position, maxStretch);
        break;
    default:
        throw GeneralException("Wrong location stored and encountered in addPossibleConnections()");
    }
}

void CrosslinkerContainer::removePossibleConnections(Crosslinker*const p_oldPartialCrosslinker, const double maxStretch)
{
    if((p_oldPartialCrosslinker->isPartial()))
    {
        throw GeneralException("The Crosslinker pointer passed to CrosslinkerContainer::removePossibleConnections() is still partial. Change it first.");
    }

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

