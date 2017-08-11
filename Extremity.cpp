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

bool Extremity::isConnected()
{
    return m_connected;
}

