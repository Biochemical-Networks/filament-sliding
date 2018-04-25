#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Propagator.hpp"

#include "MicrotubuleGraphic.hpp"
#include "PartialCrosslinkerGraphic.hpp"
#include "FullCrosslinkerGraphic.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>

class Graphics
{
private:
    // The window parameters. The parameters should be declared before the RenderWindow!
    const unsigned int m_windowWidth = 1000; // pixels
    const unsigned int m_windowHeight = 600; // pixels
    sf::RenderWindow m_window;

    // Colours:
    const sf::Color m_backGroundColour = sf::Color::White;

    // Dimensions drawings:
    const float m_circleRadius = 20.f;
    const float m_lineLength = 50.f;
    const float m_lineThickness = 10.f;
    const float m_distanceBetweenMicrotubules = 100.f;
    const float m_screenBorderThickness = 10.f;

    SystemState& m_systemState;
    Propagator& m_propagator;
    const int32_t m_timeStepsDisplayInterval;

    // Store the actual graphics in the end; these need to have all dimensions defined
    MicrotubuleGraphic m_mobileMicrotubule;
    MicrotubuleGraphic m_fixedMicrotubule;

    void draw();

    void drawPartialLinkers();

    void drawFullLinkers();

public:
    Graphics(const std::string& runName, SystemState& systemState, Propagator& propagator, const int32_t timeStepsDisplayInterval);
    ~Graphics();

    void performMainLoop(RandomGenerator& generator, Output& output);
};

#endif // GRAPHICS_HPP
