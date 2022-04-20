#include <cstdint>

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/GeneralException.hpp"
#include "filament-sliding/Site.hpp"

Site::Site(const bool isFree):
        m_isFree(isFree), mp_connectedCrosslinker(nullptr) {}

Site::~Site() {}

void Site::connectCrosslinker(
        Crosslinker& crosslinkerToConnect,
        const Crosslinker::Terminus terminusToConnect) {
#ifdef MYDEBUG
    if (!m_isFree) {
        throw GeneralException(
                "Site::connectCrosslinker() tried to connect a crosslinker "
                "that was already connected");
    }
#endif // MYDEBUG

    mp_connectedCrosslinker = &crosslinkerToConnect; // Store the address of the
                                                     // connected crosslinker
    m_connectedTerminus = terminusToConnect;

    m_isFree = false;
}

void Site::disconnectCrosslinker() {
#ifdef MYDEBUG
    if (m_isFree) {
        throw GeneralException(
                "Site::disconnectCrosslinker() tried to disconnect a "
                "crosslinker that was not connected");
    }
#endif // MYDEBUG

    mp_connectedCrosslinker = nullptr;

    m_isFree = true;
}

bool Site::isFree() const {
#ifdef MYDEBUG
    if (m_isFree && mp_connectedCrosslinker != nullptr) {
        throw GeneralException(
                "Site::isFree() was called while something was wrong");
    }
#endif // MYDEBUG

    return m_isFree;
}

bool Site::isPartial() const {
#ifdef MYDEBUG
    if (m_isFree && mp_connectedCrosslinker != nullptr) {
        throw GeneralException(
                "Site::isPartial() was called while something was wrong");
    }
#endif // MYDEBUG

    if (m_isFree) {
        return false;
    }
    else {
        return mp_connectedCrosslinker->isPartial();
    }
}

bool Site::isFull() const {
#ifdef MYDEBUG
    if (m_isFree && mp_connectedCrosslinker != nullptr) {
        throw GeneralException(
                "Site::isFull() was called while something was wrong");
    }
#endif // MYDEBUG

    if (m_isFree) {
        return false;
    }
    else {
        return mp_connectedCrosslinker->isFull();
    }
}

Crosslinker* Site::whichCrosslinkerIsBound() const {
#ifdef MYDEBUG
    if (m_isFree) {
        throw GeneralException(
                "Site::whichCrosslinkerIsBound() was called on a free "
                "Crosslinker");
    }
#endif // MYDEBUG
    return mp_connectedCrosslinker;
}
