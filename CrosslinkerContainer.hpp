#ifndef CROSSLINKERCONTAINER_HPP
#define CROSSLINKERCONTAINER_HPP

#include "Crosslinker.hpp"

#include <cstdint>
#include <vector>

class CrosslinkerContainer
{
private:
    std::vector<Crosslinker> m_crosslinkers;
public:
    CrosslinkerContainer(const int32_t nCrosslinkers, const Crosslinker& defaultCrosslinker);
    ~CrosslinkerContainer();

    Crosslinker& at(const int32_t position);
};

#endif // CROSSLINKERCONTAINER_HPP
