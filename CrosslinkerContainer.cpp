#include "CrosslinkerContainer.hpp"

#include "GeneralException/GeneralException.hpp"

#include <stdexcept>
#include <cstdint>
#include <vector>

CrosslinkerContainer::CrosslinkerContainer(const int32_t nCrosslinkers, const Crosslinker& defaultCrosslinker)
    :   m_crosslinkers(nCrosslinkers, defaultCrosslinker)
{
}

CrosslinkerContainer::~CrosslinkerContainer()
{
}

Crosslinker& CrosslinkerContainer::at(const int32_t position)
{
    try
    {
        return m_crosslinkers.at(position);
    }
    catch(std::out_of_range) // For the at function
    {
        throw GeneralException("CrosslinkerContainer.at() went out of bounds.");
    }
}
