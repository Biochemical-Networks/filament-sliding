#include "Graphics.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Propagator.hpp"
#include "Crosslinker.hpp" // For Crosslinker::Type
#include "MicrotubuleType.hpp"
#include "GeneralException/GeneralException.hpp"

#include <SFML/Graphics.hpp>

Graphics::Graphics(const std::string& runName, SystemState& systemState, Propagator& propagator, const int32_t timeStepsDisplayInterval, const int32_t updateDelayInMilliseconds)
    :   m_trueLatticeSpacing(static_cast<float>(systemState.getLatticeSpacing())),
        m_trueInitialPosition(static_cast<float>(systemState.getMicrotubulePosition())),
        m_graphicsLatticeSpacing(m_lineLength+2*m_circleRadius),
        m_mobileMicrotubuleY((static_cast<float>(m_windowHeight)-m_distanceBetweenMicrotubules)*0.5f),
        m_fixedMicrotubuleY((static_cast<float>(m_windowHeight)+m_distanceBetweenMicrotubules)*0.5f),
        m_fixedMicrotubuleX(m_screenBorderThickness),
        m_systemState(systemState),
        m_propagator(propagator),
        m_updateDelay(sf::milliseconds(updateDelayInMilliseconds)),
        m_timeStepsDisplayInterval(timeStepsDisplayInterval),
        m_pause(true), // make the system pause initially
        m_mobileMicrotubule(systemState.getNSites(MicrotubuleType::MOBILE), m_circleRadius, m_lineLength, m_lineThickness, m_circlePointCount),
        m_fixedMicrotubule(systemState.getNSites(MicrotubuleType::FIXED), m_circleRadius, m_lineLength, m_lineThickness, m_circlePointCount)
{
    //const float worldWidth = std::max({static_cast<float>(m_windowWidth),
    //                                    systemState.getNSites(MicrotubuleType::MOBILE)*m_graphicsLatticeSpacing + 2*m_screenBorderThickness,
    //                                    systemState.getNSites(MicrotubuleType::FIXED)*m_graphicsLatticeSpacing + 2*m_screenBorderThickness});

    const float centreOverlap = static_cast<float>((systemState.beginningOverlap()+0.5*systemState.overlapLength())/systemState.getLatticeSpacing())*m_graphicsLatticeSpacing;

    m_contextSettings.antialiasingLevel = 8;

    m_window.create(sf::VideoMode(m_windowWidth, m_windowHeight), "Crosslink: "+runName, sf::Style::Default, m_contextSettings); // The window title is "Crosslink: "+runName

    m_view = m_window.getDefaultView();
    m_view.setCenter(sf::Vector2f(centreOverlap, 0.5f*m_windowHeight));

    m_window.setView(m_view);

    m_mobileMicrotubule.setPosition(calculateMobileMicrotubuleX(), m_mobileMicrotubuleY);
    m_fixedMicrotubule.setPosition(m_fixedMicrotubuleX, m_fixedMicrotubuleY);

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

void Graphics::performMainLoop(RandomGenerator& generator, Output& output)
{
    sf::Clock clock; // starts the clock

    while (m_window.isOpen())
    {
        checkForWindowMovement();

        sf::Event event;
        while (m_window.pollEvent(event))
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
                m_view.reset(visibleArea);
                m_window.setView(m_view);
            }
            else if(event.type == sf::Event::MouseWheelScrolled)
            {
                m_view.zoom(std::max(1.f-event.mouseWheelScroll.delta*m_scrollStepSize,m_scrollStepSize));
                m_window.setView(m_view);
            }
            else if(event.type == sf::Event::KeyReleased)
            {
                switch(event.key.code)
                {
                case sf::Keyboard::Escape:
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
                        propagateGraphicsStep(generator, output);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        if(!m_pause && clock.getElapsedTime() > m_updateDelay)
        {
            // Restart the clock before the update: the time to propagate should be part of the delay, not extra
            clock.restart();

            propagateGraphicsStep(generator, output);
        }

        m_window.clear(m_backGroundColour);

        draw(); // draws the shapes

        m_window.display();
    }
}

void Graphics::checkForWindowMovement()
{
    const bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    const bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    if (left && !right)
    {
        m_view.move(-m_moveStepSize,0);
        m_window.setView(m_view);
    }
    else if (right && !left)
    {
        m_view.move(m_moveStepSize,0);
        m_window.setView(m_view);
    }

    const bool zoomIn = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    const bool zoomOut = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    if (zoomIn && !zoomOut)
    {

        m_view.zoom(1.f-0.1f*m_scrollStepSize); // the arrows work much faster than the scrolling, therefore we use the factor 0.1
        m_window.setView(m_view);
    }
    else if (zoomOut && !zoomIn)
    {

        m_view.zoom(1.f+0.1f*m_scrollStepSize); // the arrows work much faster than the scrolling, therefore we use the factor 0.1
        m_window.setView(m_view);
    }
}

void Graphics::update()
{
    m_mobileMicrotubule.setPosition(calculateMobileMicrotubuleX(), m_mobileMicrotubuleY);

    m_partialCrosslinkers.clear();

    updatePartialCrosslinkers(Crosslinker::Type::PASSIVE);
    updatePartialCrosslinkers(Crosslinker::Type::DUAL);
    updatePartialCrosslinkers(Crosslinker::Type::ACTIVE);

    m_fullCrosslinkers.clear();

    updateFullCrosslinkers(Crosslinker::Type::PASSIVE);
    updateFullCrosslinkers(Crosslinker::Type::DUAL);
    updateFullCrosslinkers(Crosslinker::Type::ACTIVE);

}

void Graphics::propagateGraphicsStep(RandomGenerator& generator, Output& output)
{
    m_propagator.propagateGraphicsInterval(m_systemState, generator, output, m_timeStepsDisplayInterval);

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
            m_partialCrosslinkers.back().setPosition(m_fixedMicrotubuleX+m_graphicsLatticeSpacing*boundLocation.position, m_fixedMicrotubuleY);
        }
        else
        {
            m_partialCrosslinkers.back().setPosition(calculateMobileMicrotubuleX()+m_graphicsLatticeSpacing*boundLocation.position, m_mobileMicrotubuleY);
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
            fixedPosition = sf::Vector2f(m_fixedMicrotubuleX + m_graphicsLatticeSpacing*headLocation.position, m_fixedMicrotubuleY);
            mobilePosition = sf::Vector2f(calculateMobileMicrotubuleX() + m_graphicsLatticeSpacing*tailLocation.position, m_mobileMicrotubuleY);
        }
        else
        {
            fixedPosition = sf::Vector2f(m_fixedMicrotubuleX + m_graphicsLatticeSpacing*tailLocation.position, m_fixedMicrotubuleY);
            mobilePosition = sf::Vector2f(calculateMobileMicrotubuleX() + m_graphicsLatticeSpacing*headLocation.position, m_mobileMicrotubuleY);
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
    return static_cast<float>(m_systemState.getMicrotubulePosition())/m_trueLatticeSpacing*m_graphicsLatticeSpacing+m_screenBorderThickness;
}
