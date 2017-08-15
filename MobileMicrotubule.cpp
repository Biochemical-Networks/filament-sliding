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

double MobileMicrotubule::getPosition() const
{
    return m_position;
}

void MobileMicrotubule::setPosition(const double initialPosition)
{
    m_position = initialPosition;
}
