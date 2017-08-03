#include "Microtubule.hpp"
#include <cstdint>
#include <cmath>
#include <iostream>

Microtubule::Microtubule(const double length, const double latticeSpacing)
    :   m_length(length),
        m_latticeSpacing(latticeSpacing),
        m_nSites(static_cast<int32_t>(std::floor(m_length/m_latticeSpacing)))
{
}

Microtubule::~Microtubule()
{
}
