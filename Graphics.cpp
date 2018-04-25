#include "Graphics.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Propagator.hpp"
#include "Crosslinker.hpp" // For Crosslinker::Type
#include "MicrotubuleType.hpp"
#include "GeneralException/GeneralException.hpp"

#include <SFML/Graphics.hpp>

Graphics::Graphics(const std::string& runName, SystemState& systemState, Propagator& propagator, const int32_t timeStepsDisplayInterval)
    :   m_window(sf::VideoMode(m_windowWidth, m_windowHeight), "Crosslink: "+runName), // The window title is "Crosslink: "+runName
        m_trueLatticeSpacing(static_cast<float>(systemState.getLatticeSpacing())),
        m_trueInitialPosition(static_cast<float>(systemState.getMicrotubulePosition())),
        m_graphicsLatticeSpacing(m_lineLength+2*m_circleRadius),
        m_mobileMicrotubuleY((static_cast<float>(m_windowHeight)-m_distanceBetweenMicrotubules)*0.5f),
        m_fixedMicrotubuleY((static_cast<float>(m_windowHeight)+m_distanceBetweenMicrotubules)*0.5f),
        m_fixedMicrotubuleX(m_screenBorderThickness),
        m_systemState(systemState),
        m_propagator(propagator),
        m_timeStepsDisplayInterval(timeStepsDisplayInterval),
        m_mobileMicrotubule(systemState.getNSites(MicrotubuleType::MOBILE), m_circleRadius, m_lineLength, m_lineThickness),
        m_fixedMicrotubule(systemState.getNSites(MicrotubuleType::FIXED), m_circleRadius, m_lineLength, m_lineThickness)
{
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
    while (m_window.isOpen())
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                m_window.close();
                return;
            }
        }

        m_propagator.propagateGraphicsInterval(m_systemState, generator, output, m_timeStepsDisplayInterval);

        update();

        m_window.clear(m_backGroundColour);

        draw();

        m_window.display();
    }
}

void Graphics::update()
{
    m_mobileMicrotubule.setPosition(calculateMobileMicrotubuleX(), m_mobileMicrotubuleY);

    m_partialCrosslinkers.clear();

    updatePartialCrosslinkers(Crosslinker::Type::PASSIVE);
    updatePartialCrosslinkers(Crosslinker::Type::DUAL);
    updatePartialCrosslinkers(Crosslinker::Type::ACTIVE);
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

        SiteLocation boundLocation = p_linker->getBoundLocationWhenPartiallyConnected();
        m_partialCrosslinkers.push_back(PartialCrosslinkerGraphic(m_circleRadius-m_lineThickness, m_lineThickness, 0.5f*m_distanceBetweenMicrotubules, boundWithMotor));

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

void Graphics::draw()
{
    m_window.draw(m_mobileMicrotubule);
    m_window.draw(m_fixedMicrotubule);

    for(PartialCrosslinkerGraphic& linker : m_partialCrosslinkers)
    {
        m_window.draw(linker);
    }
}

void Graphics::drawPartialLinkers()
{

}

void Graphics::drawFullLinkers()
{

}

float Graphics::calculateMobileMicrotubuleX() const
{
    return static_cast<float>(m_systemState.getMicrotubulePosition())/m_trueLatticeSpacing*m_graphicsLatticeSpacing+m_screenBorderThickness;
}
