#include "MobileMicrotubule.hpp"

MobileMicrotubule::MobileMicrotubule(const double length, const double latticeSpacing, const double diffusionConstant, const double initialPosition)
    :   Microtubule(length, latticeSpacing),
        m_diffusionConstant(diffusionConstant),
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

double MobileMicrotubule::getDiffusionConstant()
{
    return m_diffusionConstant;
}

double MobileMicrotubule::getPosition()
{
    return m_position;
}
