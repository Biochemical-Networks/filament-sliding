#ifndef PARTIALCROSSLINKERGRAPHIC_HPP
#define PARTIALCROSSLINKERGRAPHIC_HPP

#include <SFML/Graphics.hpp>

class PartialCrosslinkerGraphic : public sf::Drawable, public sf::Transformable
{
private:
    const float m_circleRadius;
    const float m_lineThickness;
    const float m_springLength;
    const sf::Color m_passiveTerminusColor = sf::Color::Blue;
    const sf::Color m_activeTerminusColor = sf::Color::Red;
    const sf::Color m_springColor = sf::Color::Black;

    sf::CircleShape m_terminus;
    sf::RectangleShape m_spring;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    PartialCrosslinkerGraphic(const float circleRadius, const float lineThickness, const float springLength, const bool boundTerminusActive, const std::size_t circlePointCount);
    virtual ~PartialCrosslinkerGraphic();
};

#endif // PARTIALCROSSLINKERGRAPHIC_HPP
