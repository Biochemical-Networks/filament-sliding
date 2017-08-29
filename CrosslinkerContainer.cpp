#include "CrosslinkerContainer.hpp"

#include "GeneralException/GeneralException.hpp"

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <algorithm>

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
    try
    {
        return m_crosslinkers.at(position);
    }
    catch(std::out_of_range) // For the at function
    {
        throw GeneralException("CrosslinkerContainer.at() went out of bounds.");
    }
}

Crosslinker& CrosslinkerContainer::connectFromFreeToPartial()
{
    // All free crosslinkers are in the same state, so it doesn't matter which one is taken. Therefore, we take the final one in the row.
    Crosslinker* p_crosslinkerToConnect = m_freeCrosslinkers.back();
    m_freeCrosslinkers.pop_back();
    m_partialCrosslinkers.push_back(p_crosslinkerToConnect);

    return *p_crosslinkerToConnect;
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
