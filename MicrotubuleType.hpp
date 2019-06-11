#ifndef MICROTUBULETYPE_HPP
#define MICROTUBULETYPE_HPP

// Contains definitions that can be used to make the labeling or the specification of Microtubules and the positions of their sites simpler.
// The definitions should not be in a class, since then either it doesn't make sense, or it can create circular dependencies

enum class MicrotubuleType
{
    FIXED,
    MOBILE
};

enum class SiteType
{
    TIP,
    BLOCKED
};

enum class BoundState
{
    BOUND,
    UNBOUND
};

// The word 'Location' will be used throughout the program to talk about the combination (microtubuleType/position/S on the microtubule).
// The word 'Position' refers to an integer giving the site number.
struct SiteLocation
{
    MicrotubuleType microtubule;
    int32_t position;
    SiteType siteType;
};


#endif //MICROTUBULETYPE_HPP
