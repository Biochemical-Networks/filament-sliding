#include <cmath>

#ifdef MYDEBUG
#include <iostream>
#endif

#include "filament-sliding/MicrotubuleType.hpp"
#include "filament-sliding/MobileMicrotubule.hpp"

MobileMicrotubule::MobileMicrotubule(
        const double length,
        const double latticeSpacing,
        const double initialPosition):
        Microtubule(MicrotubuleType::MOBILE, length, latticeSpacing),
        m_position(initialPosition) {
    m_currentAttractorPosition =
            static_cast<int32_t>(std::round(m_position / m_latticeSpacing));
#ifdef MYDEBUG
    std::cout << "Position: " << m_position
              << ", Attractor position: " << m_currentAttractorPosition << '\n';
#endif // MYDEBUG
}

MobileMicrotubule::~MobileMicrotubule() {}

void MobileMicrotubule::updatePosition(const double change) {
    m_position += change;
}

double MobileMicrotubule::getPosition() const { return m_position; }

void MobileMicrotubule::setPosition(const double initialPosition) {
    m_position = initialPosition;

    m_currentAttractorPosition =
            static_cast<int32_t>(std::round(m_position / m_latticeSpacing));
#ifdef MYDEBUG
    std::cout << "Position: " << m_position
              << ", Attractor position: " << m_currentAttractorPosition << '\n';
#endif // MYDEBUG
}

int32_t MobileMicrotubule::barrierCrossed() {
    if (m_position > (m_currentAttractorPosition + 1) * m_latticeSpacing) {
        ++m_currentAttractorPosition;
        return +1;
    }
    else if (m_position < (m_currentAttractorPosition - 1) * m_latticeSpacing) {
        --m_currentAttractorPosition;
        return -1;
    }
    else {
        return 0;
    }
}
