#ifndef EXTREMITY_HPP
#define EXTREMITY_HPP

#include <cstdint>

#include "filament-sliding/MicrotubuleType.hpp"

/* Represents one end (head or tail) of a crosslinker.
 * It keeps track if and where it is connected.
 */

class Extremity {
private:
  bool m_connected;
  MicrotubuleType m_connectedTo;
  int32_t m_sitePosition;

public:
  Extremity(const bool connected);
  ~Extremity();

  bool isConnected() const;

  SiteLocation getSiteLocation() const;

  void connect(const SiteLocation siteToConnectTo);

  void disconnect();

  void changePosition(const SiteLocation siteToConnectTo);

  MicrotubuleType getMicrotubuleConnectedTo() const;
};

#endif // EXTREMITY_HPP
