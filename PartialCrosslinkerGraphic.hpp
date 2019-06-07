#ifndef PARTIALCROSSLINKERGRAPHIC_HPP
#define PARTIALCROSSLINKERGRAPHIC_HPP

#include <SFML/Graphics.hpp>

class PartialCrosslinkerGraphic : public sf::Drawable, public sf::Transformable
{
private:
    const float m_circleRadius;
    const float m_lineThickness;
    const float m_springLength;
    const sf::Color m_passiveTerminusColorOnTip = sf::Color::Blue;
    const sf::Color m_passiveTerminusColorOnBlocked = sf::Color::Cyan;
    const sf::Color m_activeTerminusColorOnTip = sf::Color::Yellow;
    const sf::Color m_activeTerminusColorOnBlocked = sf::Color::Magenta;
    const sf::Color m_springColor = sf::Color::Black;

    sf::CircleShape m_terminus;
    sf::RectangleShape m_spring;

    sf::Color decideColor(const bool boundTerminusActive, const bool onTip) const;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    PartialCrosslinkerGraphic(const float circleRadius,
                              const float lineThickness,
                              const float springLength,
                              const bool boundTerminusActive,
                              const bool onTip,
                              const std::size_t circlePointCount);
    virtual ~PartialCrosslinkerGraphic();
};

#endif // PARTIALCROSSLINKERGRAPHIC_HPP
