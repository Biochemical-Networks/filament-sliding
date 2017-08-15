#include "Extremity.hpp"
#include <cstdint>
#include "GeneralException/GeneralException.hpp"

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

int32_t Extremity::getPosition() const
{
    if (!m_connected)
    {
        throw GeneralException("Tried to get the position of a free crosslinker extremity");
    }
    return m_sitePosition;
}

Extremity::MicrotubuleType Extremity::getMicrotubuleType() const
{
    if (!m_connected)
    {
        throw GeneralException("Tried to get the microtubule type of a free crosslinker extremity");
    }
    return m_connectedTo;
}

void Extremity::connect(const MicrotubuleType connectTo, const int32_t sitePosition)
{
    if (m_connected)
    {
        throw GeneralException("An extremity that was already connected is attempted to be connected again");
    }
    m_connected = true;
    m_connectedTo = connectTo;
    m_sitePosition = sitePosition;
}

void Extremity::changePosition(const MicrotubuleType connectedTo, const int32_t sitePosition)
{
    if (!m_connected)
    {
        throw GeneralException("An unconnected extremity was tried to be moved");
    }
    else if (connectedTo != m_connectedTo)
    {
        throw GeneralException("A connected extremity was moved to the other microtubule");
    }
    m_sitePosition = sitePosition;
}

void Extremity::disconnect()
{
    if (!m_connected)
    {
        throw GeneralException("An unconnected extremity was attempted to be disconnected");
    }
    m_connected = false;
}
