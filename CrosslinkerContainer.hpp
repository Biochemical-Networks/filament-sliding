#ifndef CROSSLINKERCONTAINER_HPP
#define CROSSLINKERCONTAINER_HPP

#include "Crosslinker.hpp"

#include <cstdint>
#include <vector>
#include <deque>

class CrosslinkerContainer
{
private:
    std::vector<Crosslinker> m_crosslinkers; // The vector manages existence of the crosslinkers.

    int32_t m_nFreeCrosslinkers;

    std::deque<Crosslinker*> m_freeCrosslinkers;
    std::deque<Crosslinker*> m_partialCrosslinkers;
    std::deque<Crosslinker*> m_fullCrosslinkers;

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
};

#endif // CROSSLINKERCONTAINER_HPP
