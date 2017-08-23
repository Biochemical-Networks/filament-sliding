#ifndef SITE_HPP
#define SITE_HPP

#include "Crosslinker.hpp"

class Site
{
private:

    bool m_isFree; // Focus on free, not occupied, because we need to ask whether we can move there (fully equivalent otherwise)

    Crosslinker *mp_connectedCrosslinker;
    Crosslinker::Terminus m_connectedTerminus;

/*    Site *m_leftFreeSite;
    Site *m_rightFreeSite;

    Extremity *mp_leftOccupiedNeighbour;
    Extremity *mp_rightOccupiedNeighbour;

    Crosslinker *mp_leftFullyConnectedCrosslinker;
    Crosslinker *mp_rightFullyConnectedCrosslinker;
*/

public:
    Site(const bool isFree);
    ~Site();

    void connectCrosslinker(Crosslinker& crosslinkerToConnect, const Crosslinker::Terminus terminusToConnect);
};

#endif // SITE_HPP
