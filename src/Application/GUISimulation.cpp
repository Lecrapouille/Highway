//==============================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of Highway.
//
// Highway is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Highway.  If not, see <http://www.gnu.org/licenses/>.
//==============================================================================

#include "Common/FileDialogs.hpp"
#include "Application/GUISimulation.hpp"
#include "Application/GUIMainMenu.hpp"
#include "Simulator/Demo.hpp"

//------------------------------------------------------------------------------
GUISimulation::GUISimulation(Application& application, std::string const& name,
                             std::string const& scenario_path)
    : Application::GUI(application, name, sf::Color::White),
      m_scenario_path(scenario_path),
      simulator(application.renderer(), m_message_bar)
{
    // SFML view for the GUI
    m_interface_view = m_renderer.getDefaultView();

    // SFML view: change the world coordinated to follow the same computations
    // than the doc "Estimation et controle pour le pilotage automatique de
    // vehicule" by Sungwoo Choi.
    m_simulation_view = m_renderer.getDefaultView();
    m_simulation_view.setSize(float(application.width()),
                              -float(application.height()));
    m_simulation_view.zoom(0.01f);
    m_renderer.setView(m_simulation_view);
}

//------------------------------------------------------------------------------
void GUISimulation::applyZoom(float const delta)
{
    constexpr float factor = 1.1f;
    constexpr float inv_factor = 1.0f / factor;

    if (delta < 0.0f)
    {
        // Factor > 1 makes the view bigger (objects appear smaller)
        m_simulation_view.zoom(factor);
        m_zoom_level *= factor;
    }
    else
    {
        // Factor < 1 makes the view smaller (objects appear bigger)
        m_simulation_view.zoom(inv_factor);
        m_zoom_level *= inv_factor;
    }
}

//------------------------------------------------------------------------------
void GUISimulation::onActivate()
{
    simulator.activate();
}

//------------------------------------------------------------------------------
void GUISimulation::onDeactivate()
{
    simulator.deactivate();
}

//------------------------------------------------------------------------------
void GUISimulation::onCreate()
{
    bool res;

    if (!m_scenario_path.empty())
    {
        // Load the simulation from a shared library.
        res = simulator.load(m_scenario_path);
    }
    else
    {
        // Hello-world simulation.
        res = simulator.load(simple_simulation_demo());
    }

    if (!res)
    {
        // Nothing to do: let inside this GUI until the user close it.
    }
}

//------------------------------------------------------------------------------
void GUISimulation::onRelease()
{
    simulator.release();
}

//------------------------------------------------------------------------------
void GUISimulation::onHandleInput()
{
    // Measurement
    Meter distance;
    static sf::Vector2<Meter> P1, P2;

    sf::Event event;

    // Get the X,Y mouse coordinates from the simulated word coordinates.
    m_renderer.setView(m_simulation_view);
    m_mouse = simulator.pixel2world(sf::Mouse::getPosition(renderer()));

    while (m_renderer.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            halt();
            break;
        // Get world's position
        case sf::Event::MouseButtonPressed:
            P1 = m_mouse;
            std::cout << "P1: (" << m_mouse.x << ", "
                      << m_mouse.y << ")" << std::endl;
            break;
        // Measure distances in meters.
        case sf::Event::MouseButtonReleased:
            P2 = m_mouse;
            distance = math::distance(P1, P2);
            if (distance >= 1.0_mm)
            {
                std::cout << "P2: (" << m_mouse.x << ", "
                          << m_mouse.y << ")" << std::endl;
                std::cout << "|P1P2| = " << math::distance(P1, P2)
                          << std::endl;
            }
            break;
        case sf::Event::MouseWheelScrolled:
            applyZoom(event.mouseWheelScroll.delta);
            break;
        case sf::Event::KeyPressed:
            // Leaving the current GUI
            if (event.key.code == sf::Keyboard::Escape)
            {
                close();
            }
            else if (event.key.code == sf::Keyboard::Space)
            {
                simulator.pause(simulator.pause() ^ true);
            }
            else if (event.key.code == sf::Keyboard::F1)
            {
                simulator.restart();
            }
            else if (event.key.code == sf::Keyboard::F5)
            {
                pfd::save_file manager("Choose the PNG file to save the screenshot",
                                       "screenshot.png", { "PNG File", "*.png" });
                std::string screenshot_filename = manager.result();
                if (!m_application.screenshot(screenshot_filename))
                {
                    m_message_bar.entry("Failed to save screenshot to file '" + screenshot_filename + "'", sf::Color::Red);
                }
                else
                {
                    m_message_bar.entry("Screenshot taken as file '" + screenshot_filename + "'", sf::Color::Green);
                }
            }
            else // propagate the key press to the simulator
            {
                simulator.reacts(event.key.code);
            }
            break;
        case sf::Event::LostFocus:
            simulator.pause(true);
            break;
        case sf::Event::GainedFocus:
            simulator.pause(false);
            break;
        default:
            break;
        }
    }
}

//------------------------------------------------------------------------------
void GUISimulation::onUpdate(const Second dt) // FIXME to be threaded
{
    if (m_state == GUISimulation::State::Running)
    {
        if (simulator.continuing())
        {
            simulator.update(dt);
        }
        else
        {
            m_state = GUISimulation::State::Closing;
            m_message_bar.append("\nHalting the simulation ...");
        }
    }
    else // Wait some seconds before closing the window
    {
        if (m_message_bar.fadded())
        {
            m_state = GUISimulation::State::Running;
            close();
        }
    }
}

//------------------------------------------------------------------------------
void GUISimulation::onDraw()
{
    // Make the camera follows the car
    m_simulation_view.setCenter(simulator.camera());

    // Draw the simulation
    simulator.drawSimulation(m_simulation_view);

    // Draw the GUI
    simulator.drawHUD(m_interface_view);
}
