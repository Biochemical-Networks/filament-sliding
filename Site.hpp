#ifndef SITE_HPP
#define SITE_HPP

/*#include "Extremity.hpp"
#include "Crosslinker.hpp"*/

class Site
{
private:

    bool m_isFree; // Focus on free, not occupied, because we need to ask whether we can move there (fully equivalent otherwise)
/*    Site *m_leftFreeSite;
    Site *m_rightFreeSite;

    Extremity *m_leftOccupiedNeighbour;
    Extremity *m_rightOccupiedNeighbour;

    Crosslinker *m_leftFullyConnectedCrosslinker;
    Crosslinker *m_rightFullyConnectedCrosslinker;


    // There are two sites on the opposing microtubule that are close to this site
    Site *m_leftOppositeSite;
    Site *m_rightOppositeSite;*/

public:
    Site(const bool isFree);
    ~Site();
};

#endif // SITE_HPP
