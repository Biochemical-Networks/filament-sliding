#ifndef FULLCROSSLINKERGRAPHIC_HPP
#define FULLCROSSLINKERGRAPHIC_HPP

#include <SFML/Graphics.hpp>

class FullCrosslinkerGraphic : public sf::Drawable, public sf::Transformable
{
private:
    const float m_circleRadius;
    const float m_lineThickness;
    const sf::Color m_passiveTerminusColor = sf::Color::Blue;
    const sf::Color m_activeTerminusColor = sf::Color::Red;
    const sf::Color m_springColor = sf::Color::Black;

    sf::CircleShape m_head;
    sf::CircleShape m_tail;
    sf::ConvexShape m_spring;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    FullCrosslinkerGraphic(const float circleRadius, const float lineThickness);
    virtual ~FullCrosslinkerGraphic();


};

#endif // FULLCROSSLINKERGRAPHIC_HPP

