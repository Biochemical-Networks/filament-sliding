#ifndef POSSIBLEHOP_HPP
#define POSSIBLEHOP_HPP

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/MicrotubuleType.hpp"

enum class HopDirection { FORWARD, BACKWARD };

struct PossiblePartialHop {
  Crosslinker *p_partialLinker;
  Crosslinker::Terminus terminusToHop; // Include the terminus, since hop rates
                                       // for motor termini can be different
  SiteLocation locationToHopTo;
  HopDirection direction;
  bool awayFromNeighbour;
};

struct PossibleFullHop {
  Crosslinker *p_fullLinker;
  Crosslinker::Terminus terminusToHop;
  SiteLocation locationToHopTo;
  HopDirection direction;
  // Keep track of the extension, since this can be updated easily
  double oldExtension; // positionOnMobile - positionOnFixed
  double newExtension; // positionOnMobile - positionOnFixed, should differ
                       // exactly one lattice spacing from oldExtension
  bool awayFromNeighbour;
};

struct FullExtremity {
  Crosslinker *p_fullLinker;
  Crosslinker::Terminus terminus;
};

#endif // POSSIBLEHOP_HPP
