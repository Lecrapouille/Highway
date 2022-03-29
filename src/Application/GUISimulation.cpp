// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

#include "Application/GUISimulation.hpp"

//------------------------------------------------------------------------------
GUISimulation::GUISimulation(Application& application)
    : Application::GUI("GUI Simulation", application.renderer()),
      m_simulator(application.renderer())
{
    // SFML view: change the world coordinated to follow the same computations
    // than the doc "Estimation et controle pour le pilotage automatique de
    // vehicule" by Sungwoo Choi.
    m_view = renderer().getDefaultView();
    m_view.setSize(float(application.width()), -float(application.height()));
    m_view.zoom(ZOOM);
    renderer().setView(m_view);
}

//------------------------------------------------------------------------------
void GUISimulation::activate()
{
    m_simulator.activate();
}

//------------------------------------------------------------------------------
void GUISimulation::deactivate()
{
    m_simulator.deactivate();
}

//------------------------------------------------------------------------------
void GUISimulation::handleInput()
{
    // Measurement
    float distance;
    static sf::Vector2f P1, P2;

    sf::Event event;

    // Get the X,Y mouse coordinates from the simulated word coordinates.
    m_mouse = m_simulator.pixel2world(sf::Mouse::getPosition(renderer()));

    while (m_running && renderer().pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            m_running = false;
            break;
        // Get world's position
        case sf::Event::MouseButtonPressed:
            P1 = m_mouse;
            std::cout << "P1: (" << m_mouse.x << ", "
                      << m_mouse.y << ") [m]" << std::endl;
            break;
        // Measure distances in meters.
        case sf::Event::MouseButtonReleased:
            P2 = m_mouse;
            distance = DISTANCE(P1, P2);
            if (distance >= 0.001f)
            {
                std::cout << "P2: (" << m_mouse.x << ", "
                          << m_mouse.y << ") [m]" << std::endl;
                std::cout << "|P1P2| = " << DISTANCE(P1, P2)
                          << " [m]" << std::endl;
            }
            break;
        case sf::Event::KeyPressed:
            // Leaving the GUI
            if (event.key.code == sf::Keyboard::Escape)
            {
                m_running = false;
            }
            else // propagate the key press to the simulator
            {
                m_simulator.reactTo(event.key.code);
            }
            break;
        default:
            break;
        }
    }
}

//------------------------------------------------------------------------------
void GUISimulation::update(const float dt) // FIXME to be threaded
{
    m_simulator.update(dt);
}

//------------------------------------------------------------------------------
void GUISimulation::draw()
{
    m_simulator.draw();

    // Make the camera follows the car
    m_view.setCenter(m_simulator.camera());
    renderer().setView(m_view);
}
