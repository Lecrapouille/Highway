//=====================================================================
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
//=====================================================================

#include "Simulator/Simulator.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/FontManager.hpp"

//------------------------------------------------------------------------------
Simulator::Simulator(sf::RenderWindow& renderer, MessageBar& message_bar)
    : m_renderer(renderer), m_message_bar(message_bar)
{
    m_message_bar.font(FontManager::instance().font("main font"));
}

//------------------------------------------------------------------------------
bool Simulator::load(Scenario const& scenario)
{
    m_scenario.name = scenario.name;
    m_scenario.create = scenario.create;
    m_scenario.halt = scenario.halt;
    m_scenario.react = scenario.react;

    // Check if it all functions are not nullptr
    if (!m_scenario)
    {
        m_error = "Failed loading the scenario because has detected nullptr functions";
        m_message_bar.entry(m_error, sf::Color::Red);
        return false;
    }

    return init();
}

//------------------------------------------------------------------------------
bool Simulator::load(std::string const& libpath)
{
    // The shared lib has been successfully opened. Now load functions.
    if (m_loader.load(libpath))
    {
        m_scenario.name = m_loader.prototype<const char* (void)>("simulation_name");
        m_scenario.create = m_loader.prototype<Car& (City&)>("create_city");
        m_scenario.halt = m_loader.prototype<bool (Simulator const&)>("halt_simulation_when");
        m_scenario.react = m_loader.prototype<void(Simulator&, size_t)>("react_to");
    }

    // Check if it all functions are not nullptr
    if (!m_scenario)
    {
        m_error = "Failed loading the scenario: " + m_loader.error();
        m_message_bar.entry(m_error, sf::Color::Red);
        return false;
    }

    return init();
}

//------------------------------------------------------------------------------
bool Simulator::autoreload()
{
    // Auto reload the scenario file if it has changed.
    if (m_loader.reloadIfChanged())
    {
        std::cerr << m_loader.prototype<const char* (void)>("simulation_name")() << std::endl;
        m_scenario.name = m_loader.prototype<const char* (void)>("simulation_name");
        m_scenario.create = m_loader.prototype<Car& (City&)>("create_city");
        m_scenario.halt = m_loader.prototype<bool (Simulator const&)>("halt_simulation_when");
        m_scenario.react = m_loader.prototype<void(Simulator&, size_t)>("react_to");

        // Check if it all functions are not nullptr
        if (!m_scenario)
        {
            m_error = "Failed loading the scenario: " + m_loader.error();
            m_message_bar.entry(m_error, sf::Color::Red);
            return false;
        }

        m_message_bar.entry("Scenario changed: reloaded", sf::Color::Yellow);
        return init();
    }

    return false;
}

//------------------------------------------------------------------------------
bool Simulator::init()
{
    // Missing call Simulator::load() or scenario has failed loaded.
    if (!m_scenario)
    {
        m_message_bar.entry("No scenario referred", sf::Color::Red);
        return false;
    }

    std::string name(m_scenario.name());

    // Set simulation name on the GUI
    m_message_bar.entry("Starting simulation " + name, sf::Color::Green);
    m_renderer.setTitle(name);

    // Create a new city from "scratch".
    m_city.reset();
    m_ego = &m_scenario.create(m_city);

    // Make by default, the camera follows the ego car.
    follow(*m_ego);

    // Clear simulation time
    m_pause = false;
    m_elpased_time = sf::Time::Zero;
    m_clock.restart();

    return true;
}

//------------------------------------------------------------------------------
void Simulator::pause(bool const state)
{
    if (m_pause == state)
        return ;

    m_pause = state;
    if (m_pause)
    {
        m_elpased_time += m_clock.getElapsedTime();
        m_message_bar.entry("Pause the simulation", sf::Color::Yellow);
    }
    else
    {
        m_message_bar.entry("Running the simulation", sf::Color::Yellow);
        m_clock.restart();
    }
}

//------------------------------------------------------------------------------
void Simulator::activate()
{
    pause(false);
}

//------------------------------------------------------------------------------
void Simulator::deactivate()
{
    pause(true);
}

//------------------------------------------------------------------------------
void Simulator::release()
{
    m_city.reset();
    m_loader.close();
    m_scenario.clear();
    m_elpased_time = sf::Time::Zero;
}

//------------------------------------------------------------------------------
void Simulator::reacts(size_t key)
{
    if ((!m_scenario) || (m_pause))
        return ;

    m_scenario.react(*this, key);
}

//------------------------------------------------------------------------------
bool Simulator::continuing() const
{
    return /*m_loader &&*/ m_scenario && (!m_scenario.halt(*this));
}

//------------------------------------------------------------------------------
void Simulator::collisions(Car& ego)
{
    bool collided = false;

    ego.clear_collided();
    for (auto& it: m_city.cars())
    {
        // Do not collide to itself
        if (it.get() == &ego)
            continue ;

        it->clear_collided();
        if (ego.collides(*it))
        {
            collided = true;
        }
    }

    if (collided)
    {
        m_message_bar.entry("Collision", sf::Color::Red);
    }
}

//------------------------------------------------------------------------------
void Simulator::update(const Second dt)
{
    // Hot reload the scenario
    autoreload();

    // User has paused the simulation ?
    if (m_pause)
    {
        m_message_bar.entry("The simulation is in pause", sf::Color::Yellow);
        return ;
    }

    // Update physics, ECU, sensors ...
    for (auto& it: m_city.cars())
    {
        it->update(dt);
    }

    // Ego vehicle
    m_ego->update(dt);
    collisions(*m_ego);

    // Make the camera follows the car
    if (m_follow != nullptr)
    {
        const sf::Vector2<Meter> p = m_follow->position();
        m_camera = sf::Vector2f(float(p.x.value()), float(p.y.value()));
    }
}

// Mauvaise facon de faire:
// class DrawableCity<City>(City& city) : m_city(city)
// { City& m_city; draw() { }
// };

//------------------------------------------------------------------------------
void Simulator::drawSimulation(sf::View const& view)
{
    m_renderer.setView(view);

    // Draw the spatial hash grid
    //Renderer::draw(m_city.grid(), m_renderer);

    // Draw the city
    for (auto const& it: m_city.roads())
    {
        Renderer::draw(*it, m_renderer);
    }

    for (auto const& it: m_city.parkings())
    {
        Renderer::draw(*it, m_renderer);
    }

    // Draw vehicle and ego
    for (auto const& it: m_city.cars())
    {
        Renderer::draw(*it, m_renderer);
    }

    // Draw ghost cars
    for (auto const& it: m_city.ghosts())
    {
        Renderer::draw(*it, m_renderer);
    }

    // Ego vehicle
    if (m_city.ego() != nullptr)
    {
        Renderer::draw(*m_city.ego(), m_renderer);
    }
}

//------------------------------------------------------------------------------
void Simulator::drawHUD(sf::View const& view)
{
    m_renderer.setView(view);
    m_message_bar.size(m_renderer.getSize());
    m_renderer.draw(m_message_bar);
}
