#ifndef SITE_HPP
#define SITE_HPP

#include "Crosslinker.hpp"
#include <cstdint>

/* Site is a simple class that only keeps track whether it is occupied or not, and if so, by which crosslinker and terminus.
 * It does not keep track of the neighbours etc, this is done in the microtubule class.
 */

class Site
{
private:

    bool m_isFree; // Focus on free, not occupied, because we need to ask whether we can move there (fully equivalent otherwise)


    Crosslinker *mp_connectedCrosslinker;
    Crosslinker::Terminus m_connectedTerminus;

public:
    Site(const bool isFree);
    ~Site();

    // Default copy constructor used to initialise vector of Sites in the Microtubule class

    void connectCrosslinker(Crosslinker& crosslinkerToConnect, const Crosslinker::Terminus terminusToConnect);

    void disconnectCrosslinker();

    bool isFree() const;

    bool isPartial() const;

    bool isFull() const;

    Crosslinker* whichCrosslinkerIsBound() const;

};

#endif // SITE_HPP
