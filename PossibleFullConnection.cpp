#include "PossibleFullConnection.hpp"

#include <algorithm>

bool operator==(const SiteLocation& lhs, const SiteLocation& rhs)
{
    return lhs.microtubule==rhs.microtubule && lhs.position==rhs.position && lhs.siteType==rhs.siteType;
}

bool operator==(const PossibleFullConnection& lhs, const PossibleFullConnection& rhs)
{
    return lhs.p_partialLinker==rhs.p_partialLinker && lhs.location==rhs.location && (std::abs(lhs.extension-rhs.extension)<0.0001);
    // Equality of the extension is too strict, since it is a floating point, so may be slightly different sometimes.
    // We demand that the calculated extensions deviate not more than 0.1 nm
}
