#include <cstdint>

#include "filament-sliding/Extremity.hpp"
#include "filament-sliding/GeneralException.hpp"
#include "filament-sliding/MicrotubuleType.hpp"

Extremity::Extremity(const bool connected): m_connected(connected) {}

Extremity::~Extremity() {}

bool Extremity::isConnected() const { return m_connected; }

SiteLocation Extremity::getSiteLocation() const {
#ifdef MYDEBUG
    if (!m_connected) {
        throw GeneralException(
                "Tried to get the location of connection of a free crosslinker "
                "extremity in Extremity::getSiteLocation()");
    }
#endif // MYDEBUG

    return SiteLocation {m_connectedTo, m_sitePosition};
}

void Extremity::connect(const SiteLocation siteToConnectTo) {
#ifdef MYDEBUG
    if (m_connected) {
        throw GeneralException(
                "An extremity that was already connected is attempted to be "
                "connected "
                "again in Extremity::connect()");
    }
#endif // MYDEBUG
    m_connected = true;
    m_connectedTo = siteToConnectTo.microtubule;
    m_sitePosition = siteToConnectTo.position;
}

void Extremity::changePosition(const SiteLocation siteToConnectTo) {
#ifdef MYDEBUG
    if (!m_connected) {
        throw GeneralException(
                "An unconnected extremity was tried to be moved in "
                "Extremity::changePosition()");
    }
    else if (siteToConnectTo.microtubule != m_connectedTo) {
        throw GeneralException(
                "A connected extremity was moved to the other "
                "microtubule in Extremity::changePosition()");
    }
#endif // MYDEBUG
    m_sitePosition = siteToConnectTo.position;
}

void Extremity::disconnect() {
#ifdef MYDEBUG
    if (!m_connected) {
        throw GeneralException(
                "An unconnected extremity was attempted to be "
                "disconnected in Extremity::disconnect()");
    }
#endif // MYDEBUG
    m_connected = false;
}

MicrotubuleType Extremity::getMicrotubuleConnectedTo() const {
#ifdef MYDEBUG
    if (!m_connected) {
        throw GeneralException(
                "Extremity::getMicrotubuleConnectedTo() was called "
                "on a disconnected extremity");
    }
#endif // MYDEBUG
    return m_connectedTo;
}
