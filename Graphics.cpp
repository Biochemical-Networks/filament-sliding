#include "Graphics.hpp"
#include "SystemState.hpp"
#include "RandomGenerator.hpp"
#include "Output.hpp"
#include "Propagator.hpp"

#include <SFML/Graphics.hpp>

Graphics::Graphics(const std::string runName, Propagator& propagator, const int32_t timeStepsDisplayInterval)
    :   m_window(sf::VideoMode(m_windowWidth, m_windowHeight), "Crosslink: "+runName), // The window title is "Crosslink: "+runName
        m_propagator(propagator),
        m_timeStepsDisplayInterval(timeStepsDisplayInterval)
{
}

Graphics::~Graphics()
{
}

void Graphics::performMainLoop(SystemState& systemState, RandomGenerator& generator, Output& output)
{
    while (m_window.isOpen())
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                m_window.close();
        }

        m_propagator.propagateGraphicsInterval(systemState, generator, output, m_timeStepsDisplayInterval);

        m_window.clear(m_backGroundColour);

        m_window.display();
    }
}

void Graphics::draw(const SystemState& systemState)
{

}


