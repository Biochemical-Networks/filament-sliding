#include "Crosslinker.hpp"
#include "GeneralException/GeneralException.hpp"

#include "Extremity.hpp"

Crosslinker::Crosslinker(const Type type, bool headConnected, bool tailConnected)
    :   m_type(type),
        m_head(headConnected),
        m_tail(tailConnected)
{
}

Crosslinker::~Crosslinker()
{
}

bool Crosslinker::isConnected() const
{
    return (m_head.isConnected()||m_tail.isConnected());
}

int32_t Crosslinker::getHeadPosition() const
{
    return m_head.getPosition();
}

int32_t Crosslinker::getTailPosition() const
{
    return m_tail.getPosition();
}

Crosslinker::Type Crosslinker::getType() const
{
    return m_type;
}

Extremity::MicrotubuleType Crosslinker::getHeadMicrotubuleType() const
{
    return m_head.getMicrotubuleType();
}

Extremity::MicrotubuleType Crosslinker::getTailMicrotubuleType() const
{
    return m_tail.getMicrotubuleType();
}

void Crosslinker::connectFromFree(const Extremity::MicrotubuleType microtubuleToConnectTo, const Terminus terminusToConnect, const int32_t position)
{
    // Check here whether it is not already connected in some way. Upon connecting an extremity, it is also checked whether those are not connected already.
    if (isConnected())
    {
        throw GeneralException("An attempt was made to connect a crosslinker that was already connected");
    }
    switch(terminusToConnect)
    {
        case Crosslinker::Terminus::HEAD:
            m_head.connect(microtubuleToConnectTo, position);
            break;
        case Crosslinker::Terminus::TAIL:
            m_tail.connect(microtubuleToConnectTo, position);
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

void Crosslinker::fullyConnectFromPartialConnection(const Extremity::MicrotubuleType microtubuleToConnectTo, const int32_t position)
{
    // Check whether it is not free. It is not necessary to check that it is fully connected, the extremities will take care of that.
    if(!isConnected())
    {
        throw GeneralException("A free crosslinker was assumed to be partially connected.");
    }
    else if (m_head.isConnected())
    {
        m_tail.connect(microtubuleToConnectTo, position);
    }
    else
    {
        m_head.connect(microtubuleToConnectTo, position);
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

void Crosslinker::getBindingPositionWhenPartiallyConnected(Extremity::MicrotubuleType& microtubuleToDisconnect, int32_t& positionToDisconnectFrom) const
{
    bool partialWithTail = (!m_head.isConnected())&&(m_tail.isConnected());
    bool partialWithHead = (m_head.isConnected())&&(!m_tail.isConnected());
    if(partialWithTail)
    {
        microtubuleToDisconnect = m_tail.getMicrotubuleType();
        positionToDisconnectFrom = m_tail.getPosition();
    }
    else if(partialWithHead)
    {
        microtubuleToDisconnect = m_head.getMicrotubuleType();
        positionToDisconnectFrom = m_head.getPosition();
    }
    else
    {
        throw GeneralException("getBindingPositionWhenPartiallyConnected() was called from a crosslinker in a different state");
    }

}


