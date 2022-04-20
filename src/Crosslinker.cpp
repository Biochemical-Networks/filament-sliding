#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/Extremity.hpp"
#include "filament-sliding/GeneralException.hpp"
#include "filament-sliding/MicrotubuleType.hpp"

Crosslinker::Crosslinker(const Type type)
    : m_type(type), m_head(false), // A crosslinker always starts as a free one,
                                   // because it requires no information
      m_tail(false) {}

Crosslinker::~Crosslinker() {}

bool Crosslinker::isConnected() const {
  return (m_head.isConnected() || m_tail.isConnected());
}

bool Crosslinker::isFree() const { return (!isConnected()); }

bool Crosslinker::isPartial() const {
  return (m_head.isConnected() != m_tail.isConnected());
}

bool Crosslinker::isFull() const {
  return (m_head.isConnected() && m_tail.isConnected());
}

Crosslinker::Type Crosslinker::getType() const { return m_type; }

SiteLocation
Crosslinker::getSiteLocationOf(const Crosslinker::Terminus terminus) const {
#ifdef MYDEBUG
  if (isFree()) {
    throw GeneralException(
        "Crosslinker::getSiteLocationOf() was called on a free linker");
  }
#endif // MYDEBUG

  switch (terminus) {
  case Crosslinker::Terminus::HEAD:
    return m_head.getSiteLocation();
    break;
  case Crosslinker::Terminus::TAIL:
    return m_tail.getSiteLocation();
    break;
  default:
    throw GeneralException(
        "Wrong terminus passed to Crosslinker::getSiteLocationOf()");
    break;
  }
}

void Crosslinker::connectFromFree(const Terminus terminusToConnect,
                                  const SiteLocation connectAt) {
// Check here whether it is not already connected in some way. Upon connecting
// an extremity, it is also checked whether those are not connected already.
#ifdef MYDEBUG
  if (isConnected()) {
    throw GeneralException(
        "An attempt was made to connect a crosslinker that was already "
        "connected in Crosslinker::connectFromFree()");
  }
#endif // MYDEBUG

  switch (terminusToConnect) {
  case Crosslinker::Terminus::HEAD:
    m_head.connect(connectAt);
    break;
  case Crosslinker::Terminus::TAIL:
    m_tail.connect(connectAt);
    break;
  default:
    throw GeneralException("An incorrect crosslinker terminus was passed to "
                           "Crosslinker::connectFromFree()");
    break;
  }
}

void Crosslinker::disconnectFromPartialConnection() {
#ifdef MYDEBUG
  if (!isPartial()) {
    throw GeneralException("Crosslinker::disconnectFromPartialConnection() was "
                           "called from a crosslinker in a different state");
  }
#endif // MYDEBUG

  if (m_tail.isConnected()) {
    m_tail.disconnect();
  } else {
    m_head.disconnect();
  }
}

void Crosslinker::fullyConnectFromPartialConnection(
    const SiteLocation connectAt) {
#ifdef MYDEBUG
  if (!isPartial()) {
    throw GeneralException("Crosslinker::fullyConnectFromPartialConnection() "
                           "was called on a non-partial linker.");
  }
#endif // MYDEBUG

  if (m_head.isConnected()) {
    m_tail.connect(connectAt);
  } else {
    m_head.connect(connectAt);
  }
}

void Crosslinker::disconnectFromFullConnection(
    const Terminus terminusToDisconnect) {
#ifdef MYDEBUG
  if (!isFull()) {
    throw GeneralException(
        "Crosslinker::disconnectFromFullConnection() was called on a "
        "crosslinker that is not fully connected");
  }
#endif // MYDEBUG

  switch (terminusToDisconnect) {
  case Terminus::TAIL:
    m_tail.disconnect();
    break;
  case Terminus::HEAD:
    m_head.disconnect();
    break;
  default:
    throw GeneralException("Crosslinker::disconnectFromFullConnection() was "
                           "passed a wrong Terminus.");
    break;
  }
}

Crosslinker::Terminus
Crosslinker::getFreeTerminusWhenPartiallyConnected() const {
#ifdef MYDEBUG
  if (!isPartial()) {
    throw GeneralException("Crosslinker::getFreeTerminusWhenPartiallyConnected("
                           ") was called on a non-partial");
  }
#endif // MYDEBUG

  if (m_head.isConnected()) {
    return Terminus::TAIL;
  } else {
    return Terminus::HEAD;
  }
}

Crosslinker::Terminus
Crosslinker::getBoundTerminusWhenPartiallyConnected() const {
#ifdef MYDEBUG
  if (!isPartial()) {
    throw GeneralException(
        "Crosslinker::getBoundTerminusWhenPartiallyConnected() was called on a "
        "non-partial");
  }
#endif // MYDEBUG

  if (m_head.isConnected()) {
    return Terminus::HEAD;
  } else {
    return Terminus::TAIL;
  }
}

Crosslinker::Terminus
Crosslinker::getTerminusOfFullOn(const MicrotubuleType microtubule) const {
#ifdef MYDEBUG
  if (!isFull()) {
    throw GeneralException(
        "Crosslinker::getTerminusOfFullOn() was called on a non-full linker");
  } else if (m_tail.getMicrotubuleConnectedTo() ==
             m_head.getMicrotubuleConnectedTo()) {
    throw GeneralException(
        "Crosslinker::getTerminusOfFullOn() was called on a linker that was "
        "connected to one microtubule twice");
  }
#endif // MYDEBUG

  if (m_head.getMicrotubuleConnectedTo() == microtubule) {
    return Terminus::HEAD;
  } else {
    return Terminus::TAIL;
  }
}

SiteLocation
Crosslinker::getLocationOfFullOn(const MicrotubuleType microtubule) const {
#ifdef MYDEBUG
  if (!isFull()) {
    throw GeneralException(
        "Crosslinker::getLocationOfFullOn() was called on a non-full linker");
  } else if (m_tail.getMicrotubuleConnectedTo() ==
             m_head.getMicrotubuleConnectedTo()) {
    throw GeneralException(
        "Crosslinker::getLocationOfFullOn() was called on a linker that was "
        "connected to one microtubule twice");
  }
#endif // MYDEBUG

  if (m_head.getMicrotubuleConnectedTo() == microtubule) {
    return m_head.getSiteLocation();
  } else {
    return m_tail.getSiteLocation();
  }
}

SiteLocation Crosslinker::getBoundLocationWhenPartiallyConnected() const {
#ifdef MYDEBUG
  if (!isPartial()) {
    throw GeneralException(
        "Crosslinker::getBoundLocationWhenPartiallyConnected() was called on a "
        "non-partial");
  }
#endif // MYDEBUG

  if (m_tail.isConnected()) {
    return m_tail.getSiteLocation();
  } else {
    return m_head.getSiteLocation();
  }
}

SiteLocation Crosslinker::getOneBoundLocationWhenFullyConnected(
    const Crosslinker::Terminus terminus) const {
#ifdef MYDEBUG
  if (!isFull()) {
    throw GeneralException("Crosslinker::getOneBoundLocationWhenFullyConnected("
                           ") was called on a non-full");
  }
#endif // MYDEBUG

  switch (terminus) {
  case Crosslinker::Terminus::TAIL:
    return m_tail.getSiteLocation();
    break;
  case Crosslinker::Terminus::HEAD:
    return m_head.getSiteLocation();
    break;
  default:
    throw GeneralException("Crosslinker::getOneBoundPositionWhenFullyConnected("
                           ") was passed a wrong Terminus.");
  }
}
