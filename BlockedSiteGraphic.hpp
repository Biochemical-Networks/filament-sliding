#ifndef BLOCKEDSITEGRAPHIC_HPP
#define BLOCKEDSITEGRAPHIC_HPP

#include <SFML/Graphics.hpp>

class BlockedSiteGraphic : public sf::Drawable, public sf::Transformable
{
private:
    const float m_circleRadius;
    const float m_lineThickness;
    const sf::Color m_blockedSiteColor = sf::Color::Green;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:

};

#endif // BLOCKEDSITEGRAPHIC_HPP
