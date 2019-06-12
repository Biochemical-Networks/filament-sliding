#include "PossibleFullConnection.hpp"

bool operator==(const SiteLocation& lhs, const SiteLocation& rhs)
{
    return lhs.microtubule==rhs.microtubule && lhs.position==rhs.position && lhs.siteType==rhs.siteType;
}

bool operator==(const PossibleFullConnection& lhs, const PossibleFullConnection& rhs)
{
    return lhs.p_partialLinker==rhs.p_partialLinker && lhs.location==rhs.location && lhs.extension==rhs.extension;
    // Is equality of the extension too strict? Is a floating point, so may be slightly different sometimes?
}
