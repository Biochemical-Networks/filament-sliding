#include "FullCrosslinkerGraphic.hpp"
#include <SFML/Graphics.hpp>

FullCrosslinkerGraphic::FullCrosslinkerGraphic(const float circleRadius, const float lineThickness)
    :   m_circleRadius(circleRadius),
        m_lineThickness(lineThickness),
        m_head(circleRadius),
        m_tail(circleRadius),
        m_spring(4) // A (thin) bar with four corners
{
    m_head.setOrigin(circleRadius,circleRadius);
    m_tail.setOrigin(circleRadius,circleRadius);
    m_spring.setFillColor(m_springColor);
}

FullCrosslinkerGraphic::~FullCrosslinkerGraphic()
{
}

void FullCrosslinkerGraphic::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the entity's transform -- combine it with the one that was passed by the caller
    states.transform *= getTransform(); // getTransform() is defined by sf::Transformable
}

