#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"

#include "Extremity.hpp"
#include "MicrotubuleType.hpp"

Crosslinker::Crosslinker(const Type type)
    :   m_type(type),
        m_head(false), // A crosslinker always starts as a free one, because it requires no information
        m_tail(false)
{
}

Crosslinker::~Crosslinker()
{
}

bool Crosslinker::isConnected() const
{
    return (m_head.isConnected()||m_tail.isConnected());
}

bool Crosslinker::isFree() const
{
    return (!isConnected());
}

bool Crosslinker::isPartial() const
{
    return (m_head.isConnected() != m_tail.isConnected());
}

bool Crosslinker::isFull() const
{
    return (m_head.isConnected()&&m_tail.isConnected());
}

Crosslinker::Type Crosslinker::getType() const
{
    return m_type;
}

/*
int32_t Crosslinker::getHeadPosition() const
{
    return m_head.getPosition();
}

int32_t Crosslinker::getTailPosition() const
{
    return m_tail.getPosition();
}

MicrotubuleType Crosslinker::getHeadMicrotubuleType() const
{
    return m_head.getMicrotubuleType();
}

MicrotubuleType Crosslinker::getTailMicrotubuleType() const
{
    return m_tail.getMicrotubuleType();
}*/

SiteLocation Crosslinker::getSiteLocationOf(const Crosslinker::Terminus terminus) const
{
    // If the specific extremity is not connected, its getSiteLocation will throw
    switch(terminus)
    {
    case Crosslinker::Terminus::HEAD:
        return m_head.getSiteLocation();
        break;
    case Crosslinker::Terminus::TAIL:
        return m_tail.getSiteLocation();
        break;
    default:
        throw GeneralException("Wrong terminus passed to getSiteLocationOf()");
        break;
    }
}

void Crosslinker::connectFromFree(const Terminus terminusToConnect, const SiteLocation connectAt)
{
    // Check here whether it is not already connected in some way. Upon connecting an extremity, it is also checked whether those are not connected already.
    if (isConnected())
    {
        throw GeneralException("An attempt was made to connect a crosslinker that was already connected");
    }
    switch(terminusToConnect)
    {
        case Crosslinker::Terminus::HEAD:
            m_head.connect(connectAt);
            break;
        case Crosslinker::Terminus::TAIL:
            m_tail.connect(connectAt);
            break;
        default:
            throw GeneralException("An incorrect crosslinker terminus was passed to Crosslinker::connectFromFree()");
            break;
    }
}

void Crosslinker::disconnectFromPartialConnection()
{
    bool partialWithTail = (!m_head.isConnected())&&(m_tail.isConnected());
    bool partialWithHead = (m_head.isConnected())&&(!m_tail.isConnected());
    if(partialWithTail)
    {
        m_tail.disconnect();
    }
    else if(partialWithHead)
    {
        m_head.disconnect();
    }
    else
    {
        throw GeneralException("disconnectFromPartialConnection() was called from a crosslinker in a different state");
    }

}

void Crosslinker::fullyConnectFromPartialConnection(const SiteLocation connectAt)
{
    // Check whether it is not free. It is not necessary to check that it is fully connected, the extremities will take care of that.
    if(!isConnected())
    {
        throw GeneralException("A free crosslinker was assumed to be partially connected.");
    }
    else if (m_head.isConnected())
    {
        m_tail.connect(connectAt);
    }
    else
    {
        m_head.connect(connectAt);
    }
}

void Crosslinker::disconnectFromFullConnection(const Terminus terminusToDisconnect)
{
    bool fullyConnected = (m_head.isConnected())&&(m_tail.isConnected());
    if(!fullyConnected)
    {
        throw GeneralException("disconnectFromFullConnection() was called on a crosslinker that is not fully connected");
    }

    switch(terminusToDisconnect)
    {
        case Terminus::TAIL:
            m_tail.disconnect();
            break;
        case Terminus::HEAD:
            m_head.disconnect();
            break;
        default:
            throw GeneralException("disconnectFromFullConnection() was passed a wrong Terminus.");
            break;
    }
}

Crosslinker::Terminus Crosslinker::getFreeTerminusWhenPartiallyConnected() const
{
    if ((m_head.isConnected())&&(!m_tail.isConnected()))
    {
        return Terminus::TAIL;
    }
    else if ((!m_head.isConnected())&&(m_tail.isConnected()))
    {
        return Terminus::HEAD;
    }
    else if ((m_head.isConnected())&&(m_tail.isConnected()))
    {
        throw GeneralException("A fully connected crosslinker was assumed to be partially connected");
    }
    else //if ((!m_head.isConnected())&&(!m_tail.isConnected()))
    {
        throw GeneralException("A free crosslinker was assumed to be partially connected");
    }
}

SiteLocation Crosslinker::getBoundLocationWhenPartiallyConnected() const
{
    bool partialWithTail = (!m_head.isConnected())&&(m_tail.isConnected());
    bool partialWithHead = (m_head.isConnected())&&(!m_tail.isConnected());
    if(partialWithTail)
    {
        return m_tail.getSiteLocation();
    }
    else if(partialWithHead)
    {
        return m_head.getSiteLocation();
    }
    else
    {
        throw GeneralException("getBoundPositionWhenPartiallyConnected() was called from a crosslinker in a different state");
    }
}

SiteLocation Crosslinker::getOneBoundLocationWhenFullyConnected(const Crosslinker::Terminus terminus) const
{
    bool fullyConnected = (m_head.isConnected())&&(m_tail.isConnected());

    if(!fullyConnected)
    {
        throw GeneralException("getOneBindingPositionWhenFullyConnected() was called on a crosslinker that is not fully connected.");
    }

    switch(terminus)
    {
        case Crosslinker::Terminus::TAIL:
            return m_tail.getSiteLocation();
            break;
        case Crosslinker::Terminus::HEAD:
            return m_head.getSiteLocation();
            break;
        default:
            throw GeneralException("getOneBoundPositionWhenFullyConnected() was passed a wrong Terminus.");
    }
}







