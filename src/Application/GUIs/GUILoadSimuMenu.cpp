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

#include "Application/GUIs/GUILoadSimuMenu.hpp"
#include "Application/Renderer/FontManager.hpp"
#include "Core/Common/FilePath.hpp"

//------------------------------------------------------------------------------
GUILoadSimuMenu::GUILoadSimuMenu(Application& application, std::string const& name)
    : Application::GUI(application, name, sf::Color::White)
{
    m_view = m_renderer.getDefaultView();
    m_text.setFont(FontManager::instance().font("main font"));
}

//------------------------------------------------------------------------------
void GUILoadSimuMenu::createListScenarios()
{
    m_scenarios.clear();

    auto const& [path, res] = FilePath::instance().find("Scenarios");
    if (!res)
    {
        LOGE("Did not find path scenario: %s", path.c_str());
        return ;
    }

    for (auto const& entry: fs::directory_iterator(FilePath::instance().expand("Scenarios")))
    {
        auto libpath = entry.path().string();
        if (libpath.substr(libpath.find_last_of(".") + 1) == SHARED_LIB_EXTENSION)
        {
            DynamicLoader dynamic_loader;
            Scenario scenario;
            if (dynamic_loader.load(entry.path(), DynamicLoader::ResolveTime::Now, DynamicLoader::Visiblity::Local) && scenario.lookup(dynamic_loader))
            {
                m_scenarios.emplace_back(entry.path(), entry.path().filename(),
                scenario.function_scenario_name());
            }
        }
    }
}

//------------------------------------------------------------------------------
void GUILoadSimuMenu::onActivate()
{
    m_renderer.setView(m_view);
    createListScenarios();
}

//------------------------------------------------------------------------------
void GUILoadSimuMenu::onDeactivate()
{}

//------------------------------------------------------------------------------
void GUILoadSimuMenu::onCreate()
{
    createListScenarios();
}

//------------------------------------------------------------------------------
void GUILoadSimuMenu::onRelease()
{}

//------------------------------------------------------------------------------
// FIXME can be factorized with GUILoadSimuMenu.cpp by adding a class Selection:
// See https://github.com/Lecrapouille/Highway/issues/5
void GUILoadSimuMenu::onHandleInput()
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
                if (m_scenarios.size() > 0u)
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
void GUILoadSimuMenu::onUpdate(const Second dt)
{}

//------------------------------------------------------------------------------
void GUILoadSimuMenu::onDraw()
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
        std::string entry = std::string(m_scenarios[i].filename)
            + ": " + m_scenarios[i].brief;
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