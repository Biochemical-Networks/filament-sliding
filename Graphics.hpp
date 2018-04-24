#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Propagator.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>

class Graphics
{
private:
    sf::RenderWindow m_window;
    const uint32_t m_windowWidth = 1000; // pixels
    const uint32_t m_windowHeight = 600; // pixels

    // Colours:
    const sf::Color m_backGroundColour = sf::Color::White;

    Propagator& m_propagator;
    const int32_t m_timeStepsDisplayInterval;

    void draw(const SystemState& systemState);

public:
    Graphics(const std::string runName, Propagator& propagator, const int32_t timeStepsDisplayInterval);
    ~Graphics();

    void performMainLoop(SystemState& systemState, RandomGenerator& generator, Output& output);
};

#endif // GRAPHICS_HPP
