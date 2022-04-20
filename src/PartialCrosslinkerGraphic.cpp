#include <SFML/Graphics.hpp>

#include "filament-sliding/PartialCrosslinkerGraphic.hpp"

PartialCrosslinkerGraphic::PartialCrosslinkerGraphic(
        const float circleRadius,
        const float lineThickness,
        const float springLength,
        const bool boundTerminusActive,
        const std::size_t circlePointCount):
        m_circleRadius(circleRadius),
        m_lineThickness(lineThickness),
        m_springLength(springLength),
        m_terminus(circleRadius, circlePointCount),
        m_spring(sf::Vector2f(
                lineThickness,
                springLength)) // A (thin) bar with four corners
{
    m_terminus.setOrigin(circleRadius, circleRadius);
    m_terminus.setFillColor(
            boundTerminusActive ? m_activeTerminusColor :
                                  m_passiveTerminusColor);
    m_spring.setOrigin(0.5f * lineThickness, 0.f);
    m_spring.setFillColor(m_springColor);
}

PartialCrosslinkerGraphic::~PartialCrosslinkerGraphic() {}

void PartialCrosslinkerGraphic::draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const {
    // apply the entity's transform -- combine it with the one that was passed
    // by the caller
    states.transform *=
            getTransform(); // getTransform() is defined by sf::Transformable
    target.draw(m_spring, states);
    target.draw(m_terminus, states);
}
