#include "Site.hpp"
#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"

#include <cstdint>

Site::Site()
    :   m_isFree(true),
        m_isBlocked(false),
        mp_connectedCrosslinker(nullptr)
{
}

Site::~Site()
{
}

void Site::connectCrosslinker(Crosslinker& crosslinkerToConnect, const Crosslinker::Terminus terminusToConnect)
{
    #ifdef MYDEBUG
    if (!m_isFree)
    {
        throw GeneralException("Site::connectCrosslinker() tried to connect a crosslinker that was already connected");
    }
    #endif // MYDEBUG

    mp_connectedCrosslinker = &crosslinkerToConnect; // Store the address of the connected crosslinker
    m_connectedTerminus = terminusToConnect;

    m_isFree = false;
}

void Site::disconnectCrosslinker()
{
    #ifdef MYDEBUG
    if (m_isFree)
    {
        throw GeneralException("Site::disconnectCrosslinker() tried to disconnect a crosslinker that was not connected");
    }
    #endif // MYDEBUG

    mp_connectedCrosslinker = nullptr;

    m_isFree = true;
}

bool Site::isFree() const
{
    #ifdef MYDEBUG
    if ((m_isFree && mp_connectedCrosslinker!=nullptr) || (!m_isFree && mp_connectedCrosslinker==nullptr))
    {
        throw GeneralException("Site::isFree() was called while something was wrong");
    }
    #endif // MYDEBUG

    return m_isFree;
}

bool Site::isPartial() const
{
    #ifdef MYDEBUG
    if ((m_isFree && mp_connectedCrosslinker!=nullptr) || (!m_isFree && mp_connectedCrosslinker==nullptr))
    {
        throw GeneralException("Site::isPartial() was called while something was wrong");
    }
    #endif // MYDEBUG

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
    #ifdef MYDEBUG
    if ((m_isFree && mp_connectedCrosslinker!=nullptr) || (!m_isFree && mp_connectedCrosslinker==nullptr))
    {
        throw GeneralException("Site::isFull() was called while something was wrong");
    }
    #endif // MYDEBUG

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
    #ifdef MYDEBUG
    if (m_isFree)
    {
        throw GeneralException("Site::whichCrosslinkerIsBound() was called on a free Crosslinker");
    }
    #endif // MYDEBUG
    return mp_connectedCrosslinker;
}

void Site::block()
{
    #ifdef MYDEBUG
    if (m_isBlocked)
    {
        throw GeneralException("Site::block() tried to block a site that could not be blocked");
    }
    #endif // MYDEBUG
    m_isBlocked = true;
}

void Site::unBlock()
{
    #ifdef MYDEBUG
    if(!m_isBlocked)
    {
        throw GeneralException("Site::unBlock() tried to unblock a site that was not blocked");
    }
    #endif // MYDEBUG
    m_isBlocked = false;
}

bool Site::isBlocked() const
{
    return m_isBlocked;
}
