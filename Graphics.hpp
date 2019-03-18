#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Propagator.hpp"
#include "MicrotubuleType.hpp"
#include "Crosslinker.hpp"

#include "MicrotubuleGraphic.hpp"
#include "PartialCrosslinkerGraphic.hpp"
#include "FullCrosslinkerGraphic.hpp"
#include "BlockedSiteGraphic.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <vector>

class Graphics
{
private:
    // The window parameters. The parameters should be declared before the RenderWindow and the X and Y positions!
    const unsigned int m_windowWidth = 1200; // pixels
    const unsigned int m_windowHeight = 800; // pixels
    const int m_windowPositionX = 200; // pixels
    const int m_windowPositionY = 100; // pixels

    // Colours:
    const sf::Color m_backGroundColour = sf::Color::White;

    // Dimensions drawings:
    const float m_circleRadius = 20.f;
    const float m_lineThickness = 10.f;
    const float m_screenBorderThickness = 30.f;
    const std::size_t m_circlePointCount = 50;
    const float m_trueLatticeSpacing;
    const float m_trueInitialPosition;
    const float m_fixedMicrotubuleX;

    float m_lineLength;
    float m_distanceBetweenMicrotubules;

    SystemState& m_systemState;
    Propagator& m_propagator;
    RandomGenerator& m_generator;
    Output& m_output;

    // Update strategy:
    const sf::Time m_updateDelay;
    const int32_t m_timeStepsDisplayInterval;
    bool m_pause;

    // Movement speed:
    const float m_moveStepSize = 10.f; // if a button is pressed, the window moves by this amount each main loop iteration. The larger the number, the faster the movement
    const float m_scrollStepSize = 0.1f; // 0 < s < 1
    const float m_keyPressedScaleStep = 0.01f;
    const float m_keyPressedStepSize = 0.1f; // 0 < s < 1

    // Store the actual graphics in the end; these need to have all dimensions defined
    MicrotubuleGraphic m_mobileMicrotubule;
    MicrotubuleGraphic m_fixedMicrotubule;

    std::vector<PartialCrosslinkerGraphic> m_partialCrosslinkers;
    std::vector<FullCrosslinkerGraphic> m_fullCrosslinkers;

    std::vector<BlockedSiteGraphic> m_blockedSites;

    sf::ContextSettings m_contextSettings;
    sf::RenderWindow m_window;
    sf::View m_view;

    void draw();

    void drawPartialLinkers();

    void drawFullLinkers();

    void drawBlockedSites();

    float calculateMobileMicrotubuleX() const;
    float calculateMobileMicrotubuleY() const;
    float calculateFixedMicrotubuleY() const;

    float calculateGraphicsLatticeSpacing() const;

    void update();
    void updatePartialCrosslinkers(const Crosslinker::Type type);
    void updateFullCrosslinkers(const Crosslinker::Type type);
    void updateBlockedSites(const MicrotubuleType type);

    void propagateGraphicsStep();

    void handleContinuousKeyPresses();

    void handleEvent(const sf::Event& event);

public:
    Graphics(const std::string& runName,
             SystemState& systemState,
             Propagator& propagator,
             RandomGenerator& generator,
             Output& output,
             const int32_t timeStepsDisplayInterval,
             const int32_t updateDelayInMilliseconds);
    ~Graphics();

    void performMainLoop();
};

#endif // GRAPHICS_HPP
