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
#include "Common/FileSystem.hpp"

//------------------------------------------------------------------------------
GUILoadSimulMenu::GUILoadSimulMenu(Application& application, std::string const& name)
    : Application::GUI(application, name, sf::Color::White)
{
    m_view = m_renderer.getDefaultView();
    m_text.setFont(FontManager::instance().font("main font"));
}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::createListScenarios()
{
    DynamicLoader loader;
    m_scenarios.clear();

    for (auto const& entry: fs::directory_iterator("/home/qq/MyGitHub/Highway/data/Scenarios"))
    {
        auto libpath = entry.path().string();
        if (libpath.substr(libpath.find_last_of(".") + 1) == SHARED_LIB_EXTENSION)
        {
            Scenario scenario;
            if (loader.load(libpath))
            {
                scenario.name = loader.prototype<const char* (void)>("simulation_name");
                scenario.create = loader.prototype<Car& (City&)>("create_city");
                scenario.halt = loader.prototype<bool (Simulator const&)>("halt_simulation_when");
                scenario.react = loader.prototype<void(Simulator&, size_t)>("react_to");
            }

            if (scenario)
            {
                ScenarioEntry e;
                e.libpath = entry.path();
                e.filename = entry.path().filename();
                e.brief = std::string(scenario.name());
                m_scenarios.push_back(std::move(e));
            }
        }
    }
}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onActivate()
{
    m_renderer.setView(m_view);
    createListScenarios();
}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onDeactivate()
{}

//------------------------------------------------------------------------------
void GUILoadSimulMenu::onCreate()
{
    createListScenarios();
}

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
                close();
            }
            else if (event.key.code == sf::Keyboard::Enter)
            {
                if (m_cursor < m_scenarios.size())
                {
                    m_application.push<GUISimulation>(
                        "GUISimulation", m_scenarios[m_cursor].libpath);
                }
            }
            else if (event.key.code == sf::Keyboard::Down)
            {
                m_cursor += 1u;
                if (m_cursor >= m_scenarios.size())
                {
                    m_cursor = 0u;
                }
            }
            else if (event.key.code == sf::Keyboard::Up)
            {
                if (m_cursor == 0u)
                {
                    m_cursor = m_scenarios.size();
                }
                if (m_cursor > 0u)
                {
                    m_cursor -= 1u;
                }
            }
            else if (event.key.code == sf::Keyboard::PageUp)
            {
                m_cursor = 0u;
            }
            else if (event.key.code == sf::Keyboard::PageDown)
            {
                m_cursor = m_scenarios.size() - 1u;
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
    // Move camera
    m_view.setCenter(sf::Vector2f(m_view.getCenter().x,
                                  float(m_renderer.getSize().y) / 2.0f +
                                  18.0f * float(m_cursor)));
    m_renderer.setView(m_view);

    // Title
    m_text.setString("Select the desired simulation and press enter");
    m_text.setPosition(0.0f, 0.0f);
    m_text.setCharacterSize(24);
    m_text.setFillColor(sf::Color::Red);
    m_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    m_renderer.draw(m_text);

    // Selections
    for (size_t i = 0u; i < m_scenarios.size(); ++i)
    {
        std::string entry(m_scenarios[i].filename + ": " + m_scenarios[i].brief);
        m_text.setString(entry);
        m_text.setPosition(24.0f, 24.0f + 4.0f + 18.0f * float(i));
        m_text.setCharacterSize(18);
        m_text.setFillColor(sf::Color::Black);
        m_text.setStyle(sf::Text::Regular);
        m_renderer.draw(m_text);
    }

    // Selector
    m_text.setString("=>");
    m_text.setPosition(0.0f, 24.0f + 4.0f + 18.0f * float(m_cursor));
    m_text.setCharacterSize(18);
    m_text.setFillColor(sf::Color::Black);
    m_text.setStyle(sf::Text::Regular);
    m_renderer.draw(m_text);
}
