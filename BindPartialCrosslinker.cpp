#include "BindPartialCrosslinker.hpp"
#include "Crosslinker.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Extremity.hpp"

#include <cstdint>

BindPartialCrosslinker::BindPartialCrosslinker(const double elementaryRate, const Crosslinker::Type typeToBind)
    :   Reaction(elementaryRate),
        m_typeToBind(typeToBind)
{
}

BindPartialCrosslinker::~BindPartialCrosslinker()
{
}

