#ifndef MICROTUBULEGRAPHIC_HPP
#define MICROTUBULEGRAPHIC_HPP

#include<SFML/Graphics.hpp>
#include <vector>
#include <cstdint>

class MicrotubuleGraphic : public sf::Drawable, public sf::Transformable
{
private:
    const int32_t m_nSites;
    const float m_circleRadius;
    const float m_lineLength;
    const float m_lineThickness;
    const sf::Color m_microtubuleColor = sf::Color::Black;
    const sf::Color m_fillColor = sf::Color::Transparent;

    // CircleShape cannot be part of a vertex array, it is one itself. Draw needs to be called on each separate circle
    std::vector<sf::CircleShape> m_circles;
    sf::VertexArray m_lines;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    MicrotubuleGraphic(const int32_t nSites, const float circleRadius, const float lineLength, const float lineThickness, const std::size_t circlePointCount);
    virtual ~MicrotubuleGraphic();
};

#endif // MICROTUBULEGRAPHIC_HPP
