#ifndef CROSSLINKERCONTAINER_HPP
#define CROSSLINKERCONTAINER_HPP

#include "Crosslinker.hpp"

#include <cstdint>
#include <vector>
#include <deque>
#include <utility> // for std::pair

class CrosslinkerContainer
{
public:
    // To prevent typing too much, define the std::pair giving the end and beginning of the deques used in this class
    typedef std::pair<std::deque<Crosslinker*>::const_iterator,std::deque<Crosslinker*>::const_iterator> beginEndDeque;
private:
    std::vector<Crosslinker> m_crosslinkers; // The vector manages existence of the crosslinkers.

    int32_t m_nFreeCrosslinkers;

    std::deque<Crosslinker*> m_freeCrosslinkers;
    std::deque<Crosslinker*> m_partialCrosslinkers;
    std::deque<Crosslinker*> m_fullCrosslinkers;

    // Stores all possible connections such that the search needs to be done once every time step
    // Needs to be updated dynamically.  After MT diffusion, it can completely change
    // Will be traversed linearly, to calculate all rates: this should happen every time step, so a vector can be accessed quickly.
    // However, to find elements or for resizing, it may be less quick. Make sure that it doesn't resize too often!
    // Added to crosslinkerContainer, because it is stored for each type of crosslinker separately.
    std::vector<possibleFullConnection> m_possibleConnections;

public:
    CrosslinkerContainer(const int32_t nCrosslinkers, const Crosslinker& defaultCrosslinker);
    ~CrosslinkerContainer();

    Crosslinker& at(const int32_t position);

    Crosslinker* connectFromFreeToPartial();

    void disconnectFromPartialToFree(Crosslinker& crosslinkerToDisconnect);

    void connectFromPartialToFull(Crosslinker& crosslinkerToConnect);

    void disconnectFromFullToPartial(Crosslinker& crosslinkerToDisconnect);

    int32_t getNCrosslinkers() const;
    int32_t getNFreeCrosslinkers() const;
    int32_t getNPartialCrosslinkers() const;
    int32_t getNFullCrosslinkers() const;

    // To find where to connect partial crosslinkers, BindPartialCrosslinker needs information on the positions of the crosslinkers.
    // To give access to the positions of the partial crosslinkers, it is easiest to pass the whole deque to the function (to avoid copying as well)
    // const_iterator doesn't allow for the modification of the container elements
    beginEndDeque getFreeCrosslinkers() const;
    beginEndDeque getPartialCrosslinkers() const;
    beginEndDeque getFullCrosslinkers() const;

    int32_t getNSitesToBindPartial() const;

    void findPossibleConnections();

};

#endif // CROSSLINKERCONTAINER_HPP
