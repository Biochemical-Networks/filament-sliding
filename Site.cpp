#include "Site.hpp"
#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"

#include <cstdint>

Site::Site(const bool isFree)
    :   m_isFree(isFree),
        mp_connectedCrosslinker(nullptr)
{
}

Site::~Site()
{
}

void Site::connectCrosslinker(Crosslinker& crosslinkerToConnect, const Crosslinker::Terminus terminusToConnect)
{
    if (!m_isFree)
    {
        throw GeneralException("Site::connectCrosslinker tried to connect a crosslinker that was already connected");
    }

    mp_connectedCrosslinker = &crosslinkerToConnect; // Store the address of the connected crosslinker
    m_connectedTerminus = terminusToConnect;

    m_isFree = false;
}

void Site::disconnectCrosslinker()
{
    if (m_isFree)
    {
        throw GeneralException("Site::disconnectCrosslinker tried to disconnect a crosslinker that was not connected");
    }

    mp_connectedCrosslinker = nullptr;

    m_isFree = true;
}

bool Site::isFree() const
{
    return m_isFree;
}

bool Site::isPartial() const
{
    if (m_isFree)
    {
        return false;
    }
    else
    {
        return mp_connectedCrosslinker->isPartial();
    }
}


bool Site::isFull() const
{
    if (m_isFree)
    {
        return false;
    }
    else
    {
        return mp_connectedCrosslinker->isFull();
    }
}



Crosslinker* Site::whichCrosslinkerIsBound() const
{
    if (m_isFree)
    {
        throw GeneralException("Site::whichCrosslinkerIsBound was called on a free Crosslinker");
    }
    return mp_connectedCrosslinker;
}
