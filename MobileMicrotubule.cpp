#include "MobileMicrotubule.hpp"
#include "MicrotubuleType.hpp"

MobileMicrotubule::MobileMicrotubule(const double length, const double latticeSpacing, const double initialPosition)
    :   Microtubule(MicrotubuleType::MOBILE, length, latticeSpacing),
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
