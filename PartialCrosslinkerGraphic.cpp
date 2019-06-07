#include "PartialCrosslinkerGraphic.hpp"
#include <SFML/Graphics.hpp>

PartialCrosslinkerGraphic::PartialCrosslinkerGraphic(const float circleRadius,
                                                     const float lineThickness,
                                                     const float springLength,
                                                     const bool boundTerminusActive,
                                                     const bool onTip,
                                                     const std::size_t circlePointCount)
    :   m_circleRadius(circleRadius),
        m_lineThickness(lineThickness),
        m_springLength(springLength),
        m_terminus(circleRadius, circlePointCount),
        m_spring(sf::Vector2f(lineThickness, springLength)) // A (thin) bar with four corners
{
    m_terminus.setOrigin(circleRadius,circleRadius);
    m_terminus.setFillColor(decideColor(boundTerminusActive, onTip));
    m_spring.setOrigin(0.5f*lineThickness, 0.f);
    m_spring.setFillColor(m_springColor);
}

PartialCrosslinkerGraphic::~PartialCrosslinkerGraphic()
{
}

void PartialCrosslinkerGraphic::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the entity's transform -- combine it with the one that was passed by the caller
    states.transform *= getTransform(); // getTransform() is defined by sf::Transformable
    target.draw(m_spring, states);
    target.draw(m_terminus, states);
}

inline sf::Color PartialCrosslinkerGraphic::decideColor(const bool boundTerminusActive, const bool onTip) const
{
    if(boundTerminusActive)
    {
        if(onTip)
        {
            return m_activeTerminusColorOnTip;
        }
        else
        {
            return m_activeTerminusColorOnBlocked;
        }
    }
    else
    {
        if(onTip)
        {
            return m_passiveTerminusColorOnTip;
        }
        else
        {
            return m_passiveTerminusColorOnBlocked;
        }
    }
}
