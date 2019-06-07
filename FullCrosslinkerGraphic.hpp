#ifndef FULLCROSSLINKERGRAPHIC_HPP
#define FULLCROSSLINKERGRAPHIC_HPP

#include <SFML/Graphics.hpp>

class FullCrosslinkerGraphic : public sf::Drawable, public sf::Transformable
{
private:
    const float m_circleRadius;
    const float m_lineThickness;
    const sf::Color m_passiveTerminusColorOnTip = sf::Color::Blue;
    const sf::Color m_passiveTerminusColorOnBlocked = sf::Color::Cyan;
    const sf::Color m_activeTerminusColorOnTip = sf::Color::Yellow;
    const sf::Color m_activeTerminusColorOnBlocked = sf::Color::Magenta;
    const sf::Color m_springColor = sf::Color::Black;

    sf::CircleShape m_mobileTerminus;
    sf::CircleShape m_fixedTerminus;
    sf::ConvexShape m_spring;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::Color decideColor(const bool boundTerminusActive, const bool onTip) const;

public:
    FullCrosslinkerGraphic(const float circleRadius,
                           const float lineThickness,
                           const bool mobileTerminusActive,
                           const bool fixedTerminusActive,
                           const bool fixedTerminusOnTip,
                           const sf::Vector2f positionOnMobile,
                           const sf::Vector2f positionOnFixed,
                           const std::size_t circlePointCount);
    virtual ~FullCrosslinkerGraphic();
};

#endif // FULLCROSSLINKERGRAPHIC_HPP

