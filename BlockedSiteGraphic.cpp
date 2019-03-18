#include "BlockedSiteGraphic.hpp"

BlockedSiteGraphic::BlockedSiteGraphic(const float circleRadius,
                                       const float lineThickness,
                                       const sf::Vector2f position,
                                       const std::size_t circlePointCount)
    :   m_circleRadius(circleRadius),
        m_lineThickness(lineThickness),
        m_site(circleRadius, circlePointCount)
{
    m_site.setOrigin(circleRadius,circleRadius);
    m_site.setFillColor(m_blockedSiteColor);
    m_site.setPosition(position);
}

BlockedSiteGraphic::~BlockedSiteGraphic()
{
}

void BlockedSiteGraphic::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the entity's transform -- combine it with the one that was passed by the caller
    states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

    target.draw(m_site, states);
}
