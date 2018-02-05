#include "CrosslinkerContainer.hpp"

#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"

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

CrosslinkerContainer::beginEndDeque CrosslinkerContainer::getFreeCrosslinkers() const
{
    return std::make_pair(m_freeCrosslinkers.begin(),m_freeCrosslinkers.end());
}

CrosslinkerContainer::beginEndDeque CrosslinkerContainer::getPartialCrosslinkers() const
{
    return std::make_pair(m_partialCrosslinkers.begin(),m_partialCrosslinkers.end());
}

CrosslinkerContainer::beginEndDeque CrosslinkerContainer::getFullCrosslinkers() const
{
    return std::make_pair(m_fullCrosslinkers.begin(),m_fullCrosslinkers.end());
}

int32_t CrosslinkerContainer::getNSitesToBindPartial() const
{

    beginEndDeque itPair = containerToCheck->getPartialCrosslinkers();

    const double mobilePosition = m_mobileMicrotubule.getPosition(); // Won't change during the subsequent for-loop

    int32_t nSitesToBindPartial = 0;

    // If there are no partially connected crosslinkers, the for body will not execute, which is how it should be
    for(std::deque<Crosslinker*>::const_iterator it = m_partialCrosslinkers.begin(); it!= m_partialCrosslinkers.end(); ++it)
    {
        // 'it' is an iterator to a pointer to a Crosslinker
        SiteLocation locationConnectedTo = (*it)->getBoundPositionWhenPartiallyConnected();

        // Check the free sites on the opposite microtubule!
        switch(locationConnectedTo.microtubule)
        {
        case MicrotubuleType::FIXED:
            nSitesToBindPartial += m_mobileMicrotubule.getNFreeSitesCloseTo(locationConnectedTo.position - mobilePosition, m_maxStretch);
            break;
        case MicrotubuleType::MOBILE:
            nSitesToBindPartial += m_fixedMicrotubule.getNFreeSitesCloseTo(locationConnectedTo.position, m_maxStretch);
            break;
        default:
            throw GeneralException("Wrong location stored and encountered in getNSitesToBindPartial()");
        }
    }
    return nSitesToBindPartial;
}




