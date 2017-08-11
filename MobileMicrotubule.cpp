#include "MobileMicrotubule.hpp"

MobileMicrotubule::MobileMicrotubule(const double length, const double latticeSpacing, const double initialPosition)
    :   Microtubule(length, latticeSpacing),
        m_position(initialPosition)
{
}

MobileMicrotubule::~MobileMicrotubule()
{
}

void MobileMicrotubule::updatePosition(const double change)
{
    m_position+=change;
}

double MobileMicrotubule::getPosition()
{
    return m_position;
}
