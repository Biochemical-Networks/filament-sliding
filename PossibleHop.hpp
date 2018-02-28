#ifndef POSSIBLEHOP_HPP
#define POSSIBLEHOP_HPP

#include "Crosslinker.hpp"
#include "MicrotubuleType.hpp"

enum class HopDirection
{
    FORWARD,
    BACKWARD
};

struct PossiblePartialHop
{
    Crosslinker* p_partialLinker;
    SiteLocation locationToHopTo;
    HopDirection direction;
};

struct PossibleFullHop
{
    Crosslinker* p_fullLinker;
    Crosslinker::Terminus terminusToHop;
    SiteLocation locationToHopTo;
    HopDirection direction;
    // Keep track of the extension, since this can be updated easily
    double oldExtension; // positionOnMobile - positionOnFixed
    double newExtension; // positionOnMobile - positionOnFixed, should differ exactly one lattice spacing from oldExtension
};

struct FullExtremity
{
    Crosslinker* p_fullLinker;
    Crosslinker::Terminus terminus;
};

#endif // POSSIBLEHOP_HPP
