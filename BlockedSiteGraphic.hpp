#ifndef BLOCKEDSITEGRAPHIC_HPP
#define BLOCKEDSITEGRAPHIC_HPP

#include <SFML/Graphics.hpp>

class BlockedSiteGraphic : public sf::Drawable, public sf::Transformable
{
private:
    const float m_circleRadius;
    const float m_lineThickness;
    const sf::Color m_blockedSiteColor = sf::Color::Green;

    sf::CircleShape m_site;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
    BlockedSiteGraphic(const float circleRadius, const float lineThickness, const sf::Vector2f position, const std::size_t circlePointCount);

    virtual ~BlockedSiteGraphic();
};

#endif // BLOCKEDSITEGRAPHIC_HPP
