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

#include "Application/GUIs/GUIMainMenu.hpp"
#include "Application/GUIs/GUILoadSimulMenu.hpp"
#include "Application/Renderer/FontManager.hpp"
//#include "Simulation/Demo.hpp"

//------------------------------------------------------------------------------
GUIMainMenu::GUIMainMenu(Application& application, std::string const& name)
    : Application::GUI(application, name, sf::Color::White)
{
    m_view = m_renderer.getDefaultView();
    m_text.setFont(FontManager::instance().font("main font"));
}

//------------------------------------------------------------------------------
void GUIMainMenu::onActivate()
{
    m_renderer.setView(m_view);
}

//------------------------------------------------------------------------------
void GUIMainMenu::onDeactivate()
{
    // FIXME do not push on stack from here else this will create inifinite loop
    // if (!m_renderer.close()) {
    //   m_application.push(m_application.gui<GUISimulation>("GUISimulation"));
    // }
}

//------------------------------------------------------------------------------
void GUIMainMenu::onCreate()
{}

//------------------------------------------------------------------------------
void GUIMainMenu::onRelease()
{}

//------------------------------------------------------------------------------
// FIXME can be factorized with GUILoadSimulMenu.cpp by adding a class Selection:
// See https://github.com/Lecrapouille/Highway/issues/5
void GUIMainMenu::onHandleInput()
{
    sf::Event event;

    while (m_renderer.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            halt();
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape)
            {
                halt();
            }
            else if (event.key.code == sf::Keyboard::Enter)
            {
                if (m_cursor == 0)
                {
                    // No scenario file given as argument => load the demo
                    // scenario.
                    m_application.push<GUISimulation>("GUISimulation");
                }
                else
                {
                    // Load the GUI shwing all scenarios installed on the
                    // system.
                    m_application.push<GUILoadSimulMenu>("GUILoadSimulMenu");
                }
            }
            else if (event.key.code == sf::Keyboard::Down)
            {
                m_cursor += 1u;
                if (m_cursor >= 2u)
                {
                    m_cursor = 0u;
                }
            }
            else if (event.key.code == sf::Keyboard::Up)
            {
                if (m_cursor == 0u)
                {
                    m_cursor = 2u;
                }
                m_cursor -= 1u;
            }
            else if (event.key.code == sf::Keyboard::PageUp)
            {
                m_cursor = 0u;
            }
            else if (event.key.code == sf::Keyboard::PageDown)
            {
                m_cursor = 1u;
            }
            break;
        default:
            break;
        }
    }
}

//------------------------------------------------------------------------------
void GUIMainMenu::onUpdate(const Second /*dt*/)
{}

//------------------------------------------------------------------------------
void GUIMainMenu::onDraw()
{
    const float y_positions[2] = {24.0f + 4.0f,
                                  24.0f + 4.0f + 18.0f + 4.0f };

    // Title
    m_text.setString("Highway: Car simulator");
    m_text.setPosition(0.0f, 0.0f);
    m_text.setCharacterSize(24);
    m_text.setFillColor(sf::Color::Red);
    m_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    m_renderer.draw(m_text);

    // Selection 0: default simulation
    m_text.setString("Launch the demo simulation");
    m_text.setPosition(24.0f, y_positions[0]);
    m_text.setCharacterSize(18);
    m_text.setFillColor(sf::Color::Black);
    m_text.setStyle(sf::Text::Regular);
    m_renderer.draw(m_text);

    // Selection 1: load simulations
    m_text.setString("Load a simulation file");
    m_text.setPosition(24.0f, y_positions[1]);
    m_text.setCharacterSize(18);
    m_text.setFillColor(sf::Color::Black);
    m_text.setStyle(sf::Text::Regular);
    m_renderer.draw(m_text);

    // Selector
    m_text.setString("=>");
    m_text.setPosition(0.0f, y_positions[m_cursor]);
    m_text.setCharacterSize(18);
    m_text.setFillColor(sf::Color::Black);
    m_text.setStyle(sf::Text::Regular);
    m_renderer.draw(m_text);
}
