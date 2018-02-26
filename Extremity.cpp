#include "Extremity.hpp"
#include <cstdint>
#include "GeneralException/GeneralException.hpp"
#include "MicrotubuleType.hpp"

Extremity::Extremity(const bool connected)
    :   m_connected(connected)
{
}

Extremity::~Extremity()
{
}

bool Extremity::isConnected() const
{
    return m_connected;
}

SiteLocation Extremity::getSiteLocation() const
{
    if (!m_connected)
    {
        throw GeneralException("Tried to get the location of connection of a free crosslinker extremity");
    }

    return SiteLocation{m_connectedTo,m_sitePosition};
}

void Extremity::connect(const SiteLocation siteToConnectTo)
{
    if (m_connected)
    {
        throw GeneralException("An extremity that was already connected is attempted to be connected again");
    }
    m_connected = true;
    m_connectedTo = siteToConnectTo.microtubule;
    m_sitePosition = siteToConnectTo.position;
}

void Extremity::changePosition(const SiteLocation siteToConnectTo)
{
    if (!m_connected)
    {
        throw GeneralException("An unconnected extremity was tried to be moved");
    }
    else if (siteToConnectTo.microtubule != m_connectedTo)
    {
        throw GeneralException("A connected extremity was moved to the other microtubule");
    }
    m_sitePosition = siteToConnectTo.position;
}

void Extremity::disconnect()
{
    if (!m_connected)
    {
        throw GeneralException("An unconnected extremity was attempted to be disconnected");
    }
    m_connected = false;
}

MicrotubuleType Extremity::getMicrotubuleConnectedTo() const
{
    return m_connectedTo;
}
