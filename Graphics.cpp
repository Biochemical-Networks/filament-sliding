#include "Graphics.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Propagator.hpp"
#include "Crosslinker.hpp" // For Crosslinker::Type
#include "MicrotubuleType.hpp"
#include "GeneralException/GeneralException.hpp"

#include <SFML/Graphics.hpp>

Graphics::Graphics(const std::string& runName,
                   SystemState& systemState,
                   Propagator& propagator,
                   RandomGenerator& generator,
                   Output& output,
                   const int32_t timeStepsDisplayInterval,
                   const int32_t updateDelayInMilliseconds)
    :   m_trueLatticeSpacing(static_cast<float>(systemState.getLatticeSpacing())),
        m_trueInitialPosition(static_cast<float>(systemState.getMicrotubulePosition())),
        m_fixedMicrotubuleX(m_screenBorderThickness),
        m_lineLength(50.f),
        m_distanceBetweenMicrotubules(200.f),
        m_systemState(systemState),
        m_propagator(propagator),
        m_generator(generator),
        m_output(output),
        m_updateDelay(sf::milliseconds(updateDelayInMilliseconds)),
        m_timeStepsDisplayInterval(timeStepsDisplayInterval),
        m_pause(true), // make the system pause initially
        m_mobileMicrotubule(systemState.getNSites(MicrotubuleType::MOBILE), m_circleRadius, m_lineLength, m_lineThickness, m_circlePointCount),
        m_fixedMicrotubule(systemState.getNSites(MicrotubuleType::FIXED), m_circleRadius, m_lineLength, m_lineThickness, m_circlePointCount),
        m_currentZoomFactor(1.0)
{
    //const float worldWidth = std::max({static_cast<float>(m_windowWidth),
    //                                    systemState.getNSites(MicrotubuleType::MOBILE)*calculateGraphicsLatticeSpacing() + 2*m_screenBorderThickness,
    //                                    systemState.getNSites(MicrotubuleType::FIXED)*calculateGraphicsLatticeSpacing() + 2*m_screenBorderThickness});

    const float centreOverlap = static_cast<float>((systemState.beginningOverlap()+0.5*systemState.overlapLength())/systemState.getLatticeSpacing())*calculateGraphicsLatticeSpacing();

    m_contextSettings.antialiasingLevel = 8;

    m_window.create(sf::VideoMode(m_windowWidth, m_windowHeight), "Crosslink: "+runName, sf::Style::Default, m_contextSettings); // The window title is "Crosslink: "+runName
    m_window.setPosition(sf::Vector2i(m_windowPositionX,m_windowPositionY));

    m_view = m_window.getDefaultView();
    m_view.setCenter(sf::Vector2f(centreOverlap, 0.5f*m_windowHeight));

    m_window.setView(m_view);

    update(); // Set all graphical entities, including the microtubules, at the right positions etc.

    // Reserve the space once, since this may prevent many initial reallocations. However, after these calls, the vectors change capacity automatically
    m_partialCrosslinkers.reserve(systemState.getPartialLinkers(Crosslinker::Type::PASSIVE).size()
                                  + systemState.getPartialLinkers(Crosslinker::Type::DUAL).size()
                                  + systemState.getPartialLinkers(Crosslinker::Type::ACTIVE).size());
    m_fullCrosslinkers.reserve(systemState.getFullLinkers(Crosslinker::Type::PASSIVE).size()
                                  + systemState.getFullLinkers(Crosslinker::Type::DUAL).size()
                                  + systemState.getFullLinkers(Crosslinker::Type::ACTIVE).size());
}

Graphics::~Graphics()
{
}

void Graphics::performMainLoop()
{
    sf::Clock clock; // starts the clock

    while (m_window.isOpen())
    {
        handleContinuousKeyPresses();

        sf::Event event;
        while (m_window.pollEvent(event))
        {
            handleEvent(event);
        }

        if(!m_pause && clock.getElapsedTime() > m_updateDelay)
        {
            // Restart the clock before the update: the time to propagate should be part of the delay, not extra
            clock.restart();

            propagateGraphicsStep();
        }

        m_window.clear(m_backGroundColour);

        draw(); // draws the shapes

        m_window.display();
    }
}

void Graphics::handleContinuousKeyPresses()
{
    const bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    const bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    if (left && !right)
    {
        m_view.move(-m_moveStepSize*m_currentZoomFactor,0);
        m_window.setView(m_view);
    }
    else if (right && !left)
    {
        m_view.move(m_moveStepSize*m_currentZoomFactor,0);
        m_window.setView(m_view);
    }

    const bool zoomIn = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    const bool zoomOut = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    if (zoomIn && !zoomOut)
    {
        const double zoomFactor = 1.f-m_keyPressedScaleStep;
        m_currentZoomFactor*=zoomFactor;
        m_view.zoom(zoomFactor); // the arrows work much faster than the scrolling, therefore we use the factor 0.1
        m_window.setView(m_view);
    }
    else if (zoomOut && !zoomIn)
    {
        const double zoomFactor = 1.f+m_keyPressedScaleStep;
        m_currentZoomFactor*=zoomFactor;
        m_view.zoom(zoomFactor); // the arrows work much faster than the scrolling, therefore we use the factor 0.1
        m_window.setView(m_view);
    }

    const bool increaseDistance = sf::Keyboard::isKeyPressed(sf::Keyboard::Add);
    const bool decreaseDistance = sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract);

    if (increaseDistance && !decreaseDistance)
    {
        m_distanceBetweenMicrotubules += m_keyPressedStepSize;

        update();
    }
    else if (decreaseDistance && !increaseDistance)
    {

        m_distanceBetweenMicrotubules -= m_keyPressedStepSize;

        update();
    }

    const bool increaseLatticeSpacing = sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp);
    const bool decreaseLatticeSpacing = sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown);

    if (increaseLatticeSpacing && !decreaseLatticeSpacing)
    {
        m_lineLength += m_keyPressedStepSize;

        m_mobileMicrotubule.setLineLength(m_lineLength);
        m_fixedMicrotubule.setLineLength(m_lineLength);

        update();
    }
    else if (decreaseLatticeSpacing && !increaseLatticeSpacing)
    {

        m_lineLength -= m_keyPressedStepSize;

        m_mobileMicrotubule.setLineLength(m_lineLength);
        m_fixedMicrotubule.setLineLength(m_lineLength);

        update();
    }
}

void Graphics::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::Closed)
    {
        m_window.close();
        return;
    }
    else if (event.type == sf::Event::Resized)
    {
        // update the view to the new size of the window
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        const float centreOverlap = static_cast<float>((m_systemState.beginningOverlap()+0.5*m_systemState.overlapLength())/m_systemState.getLatticeSpacing())*calculateGraphicsLatticeSpacing();
        m_view.reset(visibleArea);
        m_view.setCenter(sf::Vector2f(centreOverlap, 0.5f*event.size.height));
        m_window.setView(m_view);
    }
    else if(event.type == sf::Event::MouseWheelScrolled)
    {
        const double zoomFactor = std::max(1.f-event.mouseWheelScroll.delta*m_scrollStepSize,m_scrollStepSize);
        m_currentZoomFactor*=zoomFactor;
        m_view.zoom(zoomFactor);
        m_window.setView(m_view);
    }
    else if(event.type == sf::Event::KeyReleased)
    {
        switch(event.key.code)
        {
        case sf::Keyboard::Escape:
        case sf::Keyboard::Q:
            m_window.close();
            return;
            break;
        case sf::Keyboard::P:
        case sf::Keyboard::Pause:
            m_pause = !m_pause;
            break;
        case sf::Keyboard::T:
        case sf::Keyboard::Space:
            if(m_pause)
            {
                propagateGraphicsStep();
            }
            break;
        default:
            break;
        }
    }
}

void Graphics::update()
{
    // update the internal positions
    m_mobileMicrotubule.updatePositions();
    m_fixedMicrotubule.updatePositions();

    // update the global positions of the microtubules
    m_mobileMicrotubule.setPosition(calculateMobileMicrotubuleX(), calculateMobileMicrotubuleY());
    m_fixedMicrotubule.setPosition(m_fixedMicrotubuleX, calculateFixedMicrotubuleY());

    m_partialCrosslinkers.clear();

    updatePartialCrosslinkers(Crosslinker::Type::PASSIVE);
    updatePartialCrosslinkers(Crosslinker::Type::DUAL);
    updatePartialCrosslinkers(Crosslinker::Type::ACTIVE);

    m_fullCrosslinkers.clear();

    updateFullCrosslinkers(Crosslinker::Type::PASSIVE);
    updateFullCrosslinkers(Crosslinker::Type::DUAL);
    updateFullCrosslinkers(Crosslinker::Type::ACTIVE);

}

void Graphics::propagateGraphicsStep()
{
    m_propagator.propagateGraphicsInterval(m_systemState, m_generator, m_output, m_timeStepsDisplayInterval);

    update(); // updates the shapes that are stored in the Graphics class
}

void Graphics::updatePartialCrosslinkers(const Crosslinker::Type type)
{
    for(Crosslinker* p_linker : m_systemState.getPartialLinkers(type))
    {
        bool boundWithMotor;
        switch(type)
        {
        case Crosslinker::Type::PASSIVE:
            boundWithMotor = false;
            break;
        case Crosslinker::Type::DUAL:
            boundWithMotor = (p_linker->getBoundTerminusWhenPartiallyConnected() == Crosslinker::Terminus::HEAD);
            break;
        case Crosslinker::Type::ACTIVE:
            boundWithMotor = true;
            break;
        default:
            throw GeneralException("Graphics::updatePartialCrosslinkers() encountered a wrong linker type.");
            break;
        }

        const SiteLocation boundLocation = p_linker->getBoundLocationWhenPartiallyConnected();
        m_partialCrosslinkers.push_back(PartialCrosslinkerGraphic(m_circleRadius-m_lineThickness,
                                                                  m_lineThickness,
                                                                  0.5f*m_distanceBetweenMicrotubules,
                                                                  boundWithMotor,
                                                                  m_circlePointCount));

        if(boundLocation.microtubule == MicrotubuleType::FIXED)
        {
            m_partialCrosslinkers.back().rotate(180.f); // by default, it is pointing downwards
            m_partialCrosslinkers.back().setPosition(m_fixedMicrotubuleX+calculateGraphicsLatticeSpacing()*boundLocation.position, calculateFixedMicrotubuleY());
        }
        else
        {
            m_partialCrosslinkers.back().setPosition(calculateMobileMicrotubuleX()+calculateGraphicsLatticeSpacing()*boundLocation.position, calculateMobileMicrotubuleY());
        }
    }
}

void Graphics::updateFullCrosslinkers(const Crosslinker::Type type)
{
    for(Crosslinker* p_linker : m_systemState.getFullLinkers(type))
    {
    // THE FOLLOWING IS BULLSHIT, COPIED FROM THE PARTIAL VERSION!!!!
        bool fixedTerminusActive;
        bool mobileTerminusActive;
        switch(type)
        {
        case Crosslinker::Type::PASSIVE:
            fixedTerminusActive = mobileTerminusActive = false;
            break;
        case Crosslinker::Type::DUAL:
            mobileTerminusActive = (p_linker->getTerminusOfFullOn(MicrotubuleType::MOBILE) == Crosslinker::Terminus::HEAD);
            fixedTerminusActive = !mobileTerminusActive;
            break;
        case Crosslinker::Type::ACTIVE:
            fixedTerminusActive = mobileTerminusActive = true;
            break;
        default:
            throw GeneralException("Graphics::updateFullCrosslinkers() encountered a wrong linker type.");
            break;
        }

        const SiteLocation headLocation = p_linker->getOneBoundLocationWhenFullyConnected(Crosslinker::Terminus::HEAD);
        const SiteLocation tailLocation = p_linker->getOneBoundLocationWhenFullyConnected(Crosslinker::Terminus::TAIL);

        sf::Vector2f fixedPosition;
        sf::Vector2f mobilePosition;

        if(headLocation.microtubule == MicrotubuleType::FIXED)
        {
            fixedPosition = sf::Vector2f(m_fixedMicrotubuleX + calculateGraphicsLatticeSpacing()*headLocation.position, calculateFixedMicrotubuleY());
            mobilePosition = sf::Vector2f(calculateMobileMicrotubuleX() + calculateGraphicsLatticeSpacing()*tailLocation.position, calculateMobileMicrotubuleY());
        }
        else
        {
            fixedPosition = sf::Vector2f(m_fixedMicrotubuleX + calculateGraphicsLatticeSpacing()*tailLocation.position, calculateFixedMicrotubuleY());
            mobilePosition = sf::Vector2f(calculateMobileMicrotubuleX() + calculateGraphicsLatticeSpacing()*headLocation.position, calculateMobileMicrotubuleY());
        }

        m_fullCrosslinkers.push_back(FullCrosslinkerGraphic(m_circleRadius-m_lineThickness,
                                                                   m_lineThickness,
                                                                   mobileTerminusActive,
                                                                   fixedTerminusActive,
                                                                   mobilePosition,
                                                                   fixedPosition,
                                                                   m_circlePointCount));
    }
}

void Graphics::draw()
{
    m_window.draw(m_mobileMicrotubule);
    m_window.draw(m_fixedMicrotubule);

    drawPartialLinkers();

    drawFullLinkers();
}

void Graphics::drawPartialLinkers()
{
    for(PartialCrosslinkerGraphic& linker : m_partialCrosslinkers)
    {
        m_window.draw(linker);
    }
}

void Graphics::drawFullLinkers()
{
    for(FullCrosslinkerGraphic& linker : m_fullCrosslinkers)
    {
        m_window.draw(linker);
    }
}

float Graphics::calculateMobileMicrotubuleX() const
{
    return static_cast<float>(m_systemState.getMicrotubulePosition())/m_trueLatticeSpacing*calculateGraphicsLatticeSpacing()+m_screenBorderThickness;
}

float Graphics::calculateMobileMicrotubuleY() const
{
    return (static_cast<float>(m_windowHeight)-m_distanceBetweenMicrotubules)*0.5f;
}

float Graphics::calculateFixedMicrotubuleY() const
{
    return (static_cast<float>(m_windowHeight)+m_distanceBetweenMicrotubules)*0.5f;
}

float Graphics::calculateGraphicsLatticeSpacing() const
{
    return m_lineLength+2*m_circleRadius;
}
