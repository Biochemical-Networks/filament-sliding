#include "MobileMicrotubule.hpp"
#include "MicrotubuleType.hpp"

#include <cmath>

MobileMicrotubule::MobileMicrotubule(const double length, const double latticeSpacing, const double initialPosition)
    :   Microtubule(MicrotubuleType::MOBILE, length, latticeSpacing),
        m_position(initialPosition)
{
    m_currentAttractorPosition = static_cast<int32_t>(std::round(initialPosition/m_latticeSpacing));
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

bool MobileMicrotubule::barrierCrossed()
{
    if(m_position > (m_currentAttractorPosition+1)*m_latticeSpacing)
    {
        ++m_currentAttractorPosition;
        return true;
    }
    else if(m_position < (m_currentAttractorPosition-1)*m_latticeSpacing)
    {
        --m_currentAttractorPosition;
        return true;
    }
    else
    {
        return false;
    }
}
