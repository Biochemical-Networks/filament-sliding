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
#include <vector>

class Graphics
{
private:
    // The window parameters. The parameters should be declared before the RenderWindow and the X and Y positions!
    const unsigned int m_windowWidth = 1000; // pixels
    const unsigned int m_windowHeight = 600; // pixels

    // Colours:
    const sf::Color m_backGroundColour = sf::Color::White;

    // Dimensions drawings:
    const float m_circleRadius = 20.f;
    const float m_lineLength = 50.f;
    const float m_lineThickness = 10.f;
    const float m_distanceBetweenMicrotubules = 100.f;
    const float m_screenBorderThickness = 30.f;
    const std::size_t m_circlePointCount = 50;
    const float m_trueLatticeSpacing;
    const float m_trueInitialPosition;
    const float m_graphicsLatticeSpacing;
    const float m_mobileMicrotubuleY;
    const float m_fixedMicrotubuleY;
    const float m_fixedMicrotubuleX;

    SystemState& m_systemState;
    Propagator& m_propagator;
    const int32_t m_timeStepsDisplayInterval;

    // Store the actual graphics in the end; these need to have all dimensions defined
    MicrotubuleGraphic m_mobileMicrotubule;
    MicrotubuleGraphic m_fixedMicrotubule;

    std::vector<PartialCrosslinkerGraphic> m_partialCrosslinkers;
    std::vector<FullCrosslinkerGraphic> m_fullCrosslinkers;

    sf::ContextSettings m_contextSettings;
    sf::RenderWindow m_window;
    sf::View m_view;

    void draw();

    void drawPartialLinkers();

    void drawFullLinkers();

    float calculateMobileMicrotubuleX() const;

    void update();
    void updatePartialCrosslinkers(const Crosslinker::Type type);
    void updateFullCrosslinkers(const Crosslinker::Type type);

public:
    Graphics(const std::string& runName, SystemState& systemState, Propagator& propagator, const int32_t timeStepsDisplayInterval);
    ~Graphics();

    void performMainLoop(RandomGenerator& generator, Output& output);
};

#endif // GRAPHICS_HPP
