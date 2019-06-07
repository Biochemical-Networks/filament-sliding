#include "FullCrosslinkerGraphic.hpp"
#include <SFML/Graphics.hpp>

FullCrosslinkerGraphic::FullCrosslinkerGraphic(const float circleRadius,
                                               const float lineThickness,
                                               const bool mobileTerminusActive,
                                               const bool fixedTerminusActive,
                                               const bool fixedTerminusOnTip,
                                               const sf::Vector2f positionOnMobile,
                                               const sf::Vector2f positionOnFixed,
                                               const std::size_t circlePointCount)
    :   m_circleRadius(circleRadius),
        m_lineThickness(lineThickness),
        m_mobileTerminus(circleRadius, circlePointCount),
        m_fixedTerminus(circleRadius, circlePointCount),
        m_spring(4) // A (thin) bar with four corners
{
    m_mobileTerminus.setOrigin(circleRadius,circleRadius);
    m_mobileTerminus.setFillColor(decideColor(mobileTerminusActive, true)); // The mobile (actin) filament is always in the tip state
    m_mobileTerminus.setPosition(positionOnMobile);

    m_fixedTerminus.setOrigin(circleRadius,circleRadius);
    m_fixedTerminus.setFillColor(decideColor(fixedTerminusActive, fixedTerminusOnTip));
    m_fixedTerminus.setPosition(positionOnFixed);

    m_spring.setFillColor(m_springColor);
    m_spring.setPoint(0, positionOnMobile-sf::Vector2f(0.5f*lineThickness,0.f));
    m_spring.setPoint(1, positionOnMobile+sf::Vector2f(0.5f*lineThickness,0.f));
    m_spring.setPoint(2, positionOnFixed+sf::Vector2f(0.5f*lineThickness,0.f));
    m_spring.setPoint(3, positionOnFixed-sf::Vector2f(0.5f*lineThickness,0.f));
}

FullCrosslinkerGraphic::~FullCrosslinkerGraphic()
{
}

void FullCrosslinkerGraphic::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the entity's transform -- combine it with the one that was passed by the caller
    states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

    target.draw(m_spring, states);
    target.draw(m_mobileTerminus, states);
    target.draw(m_fixedTerminus, states);
}

inline sf::Color FullCrosslinkerGraphic::decideColor(const bool boundTerminusActive, const bool onTip) const
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

