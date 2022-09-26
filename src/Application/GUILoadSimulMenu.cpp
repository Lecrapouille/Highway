//==============================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
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
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//==============================================================================

#include "Application/GUILoadSimulMenu.hpp"
#include "Renderer/FontManager.hpp"

//------------------------------------------------------------------------------
GUILoadSimulMenu::GUILoadSimulMenu(Application& application, const char* name)
    : Application::GUI(application, name, sf::Color::White)
{
    m_view = m_renderer.getDefaultView();

    m_text.setFont(FontManager::instance().font("main font"));
    m_text.setString("Select the desired simulation and press enter");
    m_text.setCharacterSize(24);
    m_text.setFillColor(sf::Color::Red);
    m_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onActivate()
{
    m_renderer.setView(m_view);
}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onDeactivate()
{
}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onCreate()
{}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onRelease()
{}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onHandleInput()
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
            else if (event.key.code == sf::Keyboard::Space)
            {
                m_application.push(m_application.gui<GUISimulation>("GUISimulation"));
            }
            break;
        default:
            break;
        }
    }
}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onUpdate(const float dt)
{}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onDraw()
{
    m_renderer.draw(m_text);
}
