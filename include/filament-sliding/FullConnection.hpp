#ifndef FULLCONNECTION_HPP
#define FULLCONNECTION_HPP

#include "filament-sliding/Crosslinker.hpp"
#include "filament-sliding/MicrotubuleType.hpp"

// Defined struct to group data about a fully connected crosslinker
struct FullConnection {
  Crosslinker *p_fullLinker;
  // Extension is the position of the connection on the mobile microtubule minus
  // the position on the fixed microtubule. Hence, is positive if fixed
  // connection is closest to the origin
  double extension;
};

// Used for checking if possible connections cross existing full connections
struct FullConnectionLocations {
  Crosslinker *p_fullLinker;
  SiteLocation locationNextToPartial;
  SiteLocation locationOppositeToPartial;
};

#endif // FULLCONNECTION_HPP
