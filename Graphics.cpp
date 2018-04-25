#include "Graphics.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Propagator.hpp"
#include "Crosslinker.hpp" // For Crosslinker::Type
#include "MicrotubuleType.hpp"

#include <SFML/Graphics.hpp>

Graphics::Graphics(const std::string& runName, SystemState& systemState, Propagator& propagator, const int32_t timeStepsDisplayInterval)
    :   m_window(sf::VideoMode(m_windowWidth, m_windowHeight), "Crosslink: "+runName), // The window title is "Crosslink: "+runName
        m_systemState(systemState),
        m_propagator(propagator),
        m_timeStepsDisplayInterval(timeStepsDisplayInterval),
        m_trueLatticeSpacing(static_cast<float>(systemState.getLatticeSpacing())),
        m_trueInitialPosition(static_cast<float>(systemState.getMicrotubulePosition())),
        m_graphicsLatticeSpacing(m_lineLength+2*m_circleRadius),
        m_mobileMicrotubuleY(static_cast<float>(m_windowHeight)/3),
        m_fixedMicrotubuleY(2*m_mobileMicrotubuleY),
        m_fixedMicrotubuleX(-(m_trueInitialPosition/m_trueLatticeSpacing-1.f)*m_graphicsLatticeSpacing+m_screenBorderThickness),
        m_mobileMicrotubule(systemState.getNSites(MicrotubuleType::MOBILE), m_circleRadius, m_lineLength, m_lineThickness),
        m_fixedMicrotubule(systemState.getNSites(MicrotubuleType::FIXED), m_circleRadius, m_lineLength, m_lineThickness)
{
    m_mobileMicrotubule.setPosition(calculateMobileMicrotubuleX(), m_mobileMicrotubuleY);
    m_fixedMicrotubule.setPosition(m_fixedMicrotubuleX, m_fixedMicrotubuleY);
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
                m_window.close();
        }

        m_propagator.propagateGraphicsInterval(m_systemState, generator, output, m_timeStepsDisplayInterval);

        m_window.clear(m_backGroundColour);

        draw();

        m_window.display();
    }
}

void Graphics::draw()
{
    const double mobilePosition = m_systemState.getMicrotubulePosition();


}

void Graphics::drawPartialLinkers()
{

}

void Graphics::drawFullLinkers()
{

}

float Graphics::calculateMobileMicrotubuleX() const
{
    return ((static_cast<float>(m_systemState.getMicrotubulePosition())-m_trueInitialPosition)/m_trueLatticeSpacing+1.f)*m_graphicsLatticeSpacing+m_screenBorderThickness;
}
