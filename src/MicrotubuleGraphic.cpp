#include <cstdint>

#include <SFML/Graphics.hpp>

#include "filament-sliding/MicrotubuleGraphic.hpp"

MicrotubuleGraphic::MicrotubuleGraphic(const int32_t nSites,
                                       const float circleRadius,
                                       const float lineLength,
                                       const float lineThickness,
                                       const std::size_t circlePointCount)
    : m_nSites(nSites), m_circleRadius(circleRadius),
      m_lineThickness(lineThickness), m_lineLength(lineLength),
      m_circles(nSites, sf::CircleShape(circleRadius, circlePointCount)),
      m_lines(sf::Quads, 4 * (nSites - 1)) // The number of vertices of a quad
                                           // is 4; nSites-1 quads are needed
{
  for (int32_t pos = 0; pos < m_nSites; ++pos) {
    m_circles[pos].setOrigin(
        m_circleRadius,
        m_circleRadius); // set the origins of the circles to their centre
    m_circles[pos].setFillColor(m_fillColor);
    m_circles[pos].setOutlineThickness(
        -m_lineThickness); // A negative value means that the outline goes
                           // inward, and the radius stays the same
    m_circles[pos].setOutlineColor(m_microtubuleColor);
    m_circles[pos].setPosition(pos * (m_lineLength + 2 * m_circleRadius), 0.f);
  }
  for (int32_t pos = 0; pos < m_nSites - 1;
       ++pos) // The number of lines equals nSites-1
  {
    m_lines[4 * pos + 0].position =
        sf::Vector2f(pos * (m_lineLength + 2 * m_circleRadius) +
                         m_circleRadius - 0.5f * m_lineThickness,
                     0.5f * m_lineThickness);
    m_lines[4 * pos + 0].color = m_microtubuleColor;
    m_lines[4 * pos + 1].position =
        sf::Vector2f(pos * (m_lineLength + 2 * m_circleRadius) +
                         m_circleRadius + m_lineLength + 0.5f * m_lineThickness,
                     0.5f * m_lineThickness);
    m_lines[4 * pos + 1].color = m_microtubuleColor;
    m_lines[4 * pos + 2].position =
        sf::Vector2f(pos * (m_lineLength + 2 * m_circleRadius) +
                         m_circleRadius + m_lineLength + 0.5f * m_lineThickness,
                     -0.5f * m_lineThickness);
    m_lines[4 * pos + 2].color = m_microtubuleColor;
    m_lines[4 * pos + 3].position =
        sf::Vector2f(pos * (m_lineLength + 2 * m_circleRadius) +
                         m_circleRadius - 0.5f * m_lineThickness,
                     -0.5f * m_lineThickness);
    m_lines[4 * pos + 3].color = m_microtubuleColor;
  }
}

MicrotubuleGraphic::~MicrotubuleGraphic() {}

void MicrotubuleGraphic::draw(sf::RenderTarget &target,
                              sf::RenderStates states) const {
  // apply the entity's transform -- combine it with the one that was passed by
  // the caller
  states.transform *=
      getTransform(); // getTransform() is defined by sf::Transformable

  for (const sf::CircleShape &circle : m_circles) {
    target.draw(circle, states);
  }
  target.draw(m_lines, states);
}

void MicrotubuleGraphic::updatePositions() {
  for (int32_t pos = 0; pos < m_nSites; ++pos) {
    m_circles[pos].setPosition(pos * (m_lineLength + 2 * m_circleRadius), 0.f);
  }
  for (int32_t pos = 0; pos < m_nSites - 1;
       ++pos) // The number of lines equals nSites-1
  {
    m_lines[4 * pos + 0].position =
        sf::Vector2f(pos * (m_lineLength + 2 * m_circleRadius) +
                         m_circleRadius - 0.5f * m_lineThickness,
                     0.5f * m_lineThickness);
    m_lines[4 * pos + 1].position =
        sf::Vector2f(pos * (m_lineLength + 2 * m_circleRadius) +
                         m_circleRadius + m_lineLength + 0.5f * m_lineThickness,
                     0.5f * m_lineThickness);
    m_lines[4 * pos + 2].position =
        sf::Vector2f(pos * (m_lineLength + 2 * m_circleRadius) +
                         m_circleRadius + m_lineLength + 0.5f * m_lineThickness,
                     -0.5f * m_lineThickness);
    m_lines[4 * pos + 3].position =
        sf::Vector2f(pos * (m_lineLength + 2 * m_circleRadius) +
                         m_circleRadius - 0.5f * m_lineThickness,
                     -0.5f * m_lineThickness);
  }
}

void MicrotubuleGraphic::setLineLength(const float lineLength) {
  m_lineLength = lineLength;
  updatePositions();
}
