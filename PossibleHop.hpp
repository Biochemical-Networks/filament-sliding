#ifndef POSSIBLEHOP_HPP
#define POSSIBLEHOP_HPP

#include "Crosslinker.hpp"
#include "MicrotubuleType.hpp"

struct PossiblePartialHop
{
    Crosslinker* p_partialLinker;
    SiteLocation locationToHopTo;
};

struct PossibleFullHop
{
    Crosslinker* p_fullLinker;
    Crosslinker::Terminus terminusToHop;
    SiteLocation locationToHopTo;
    // Keep track of the extension, since this can be updated easily
    double oldExtension; // positionOnMobile - positionOnFixed
    double newExtension; // positionOnMobile - positionOnFixed, should differ exactly one lattice spacing from oldExtension
};

#endif // POSSIBLEHOP_HPP
