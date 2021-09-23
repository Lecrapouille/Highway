// 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

#include "main.hpp"
#include "World/Blueprints.hpp"
#include "Renderer/Renderer.hpp"
#include <iostream>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

//------------------------------------------------------------------------------
// Note the view coordinates are
//   ^ Y
//   |
//   +------> X
// Changed through view.setSize() to follow the same computations than the doc
// "Estimation et controle pour le pilotage automatique de vehicule" by Sungwoo Choi
GUISimulation::GUISimulation(Application& application)
    : IGUIStates("Car Simulation", application.renderer())
{
    // SFML view
    m_view = renderer().getDefaultView();
    m_view.setSize(float(application.width()), -float(application.height()));
    m_view.zoom(ZOOM);
    renderer().setView(m_view);
}

//------------------------------------------------------------------------------
void GUISimulation::activate()
{
    // Do nothing: world creation is done through keyboard key pressed events
}

//------------------------------------------------------------------------------
void GUISimulation::deactivate()
{
    m_simulation.clear();
}

//------------------------------------------------------------------------------
void GUISimulation::handleInput()
{
    // Measurement
    float distance;
    static sf::Vector2f P1, P2;

    sf::Event event;

    // Get the X,Y mouse coordinates from the simulated word coordinates.
    m_mouse = world(sf::Mouse::getPosition(renderer()));

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
            distance = SFDISTANCE(P1, P2);
            if (distance >= 0.001f)
            {
                std::cout << "P2: (" << m_mouse.x << ", "
                          << m_mouse.y << ") [m]" << std::endl;
                std::cout << "|P1P2| = " << SFDISTANCE(P1, P2)
                          << " [m]" << std::endl;
            }
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape)
            {
                m_running = false;
            }
            else if (event.key.code == sf::Keyboard::Right)
            {
                if (m_simulation.m_ego != nullptr)
                {
                    // TODO TURNING INDICATOR: Left/right
                    std::cout << "TURNING INDICATOR: START PARKING" << std::endl;
                    m_simulation.m_ego->clignotant(m_simulation.m_ego->clignotant() ^ true);
                }
            }
            else if (event.key.code == sf::Keyboard::A)
            {
                std::cout << "ENTERING BACKWARD PARALLEL" << std::endl;
                m_simulation.createWorld(0, true);
            }
            else if (event.key.code == sf::Keyboard::Z)
            {
                std::cout << "LEAVING BACKWARD PARALLEL" << std::endl;
                m_simulation.createWorld(0, false);
            }
            else if (event.key.code == sf::Keyboard::E)
            {
                std::cout << "ENTERING BACKWARD DIAGONAL 45" << std::endl;
                m_simulation.createWorld(45, true);
            }
            else if (event.key.code == sf::Keyboard::R)
            {
                std::cout << "LEAVING BACKWARD DIAGONAL 45" << std::endl;
                m_simulation.createWorld(45, false);
            }
            else if (event.key.code == sf::Keyboard::T)
            {
                std::cout << "ENTERING BACKWARD DIAGONAL 60" << std::endl;
                m_simulation.createWorld(60, true);
            }
            else if (event.key.code == sf::Keyboard::Y)
            {
                std::cout << "LEAVING BACKWARD DIAGONAL 60" << std::endl;
                m_simulation.createWorld(60, false);
            }
            else if (event.key.code == sf::Keyboard::U)
            {
                std::cout << "ENTERING BACKWARD DIAGONAL 75" << std::endl;
                m_simulation.createWorld(75, true);
            }
            else if (event.key.code == sf::Keyboard::I)
            {
                std::cout << "LEAVING BACKWARD DIAGONAL 75" << std::endl;
                m_simulation.createWorld(75, false);
            }
            else if (event.key.code == sf::Keyboard::O)
            {
                std::cout << "ENTERING BACKWARD PERPENDICULAR" << std::endl;
                m_simulation.createWorld(90, true);
            }
            else if (event.key.code == sf::Keyboard::P)
            {
                std::cout << "LEAVING BACKWARD PERPENDICULAR" << std::endl;
                m_simulation.createWorld(90, false);
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
    m_simulation.update(dt);
}

//------------------------------------------------------------------------------
void GUISimulation::draw(const float /*dt*/)
{
    m_simulation.draw(renderer(), m_view);
}

// -----------------------------------------------------------------------------
int main()
{
    Application app(WINDOW_WIDTH, WINDOW_HEIGHT, "Auto Parking");
    GUISimulation gui(app);
    gui.bgColor = sf::Color(255,255,255,255);

    try
    {
        app.push(gui);
        app.loop();
    }
    catch (std::string const& msg)
    {
        std::cerr << "Fatal: " << msg << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
