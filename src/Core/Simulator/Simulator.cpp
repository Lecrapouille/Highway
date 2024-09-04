//=====================================================================
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
//=====================================================================

#include "Core/Simulator/Simulator.hpp"
#include "MyLogger/Logger.hpp"

//------------------------------------------------------------------------------
Simulator::Simulator()//sf::RenderWindow& renderer, MessageBar& message_bar)
//    : m_renderer(renderer), m_message_bar(message_bar)
{
}

//------------------------------------------------------------------------------
bool Simulator::load(fs::path const& libpath)
{
    LOGI("Simulator loads '%s'", libpath.c_str());

    // The shared lib has been successfully opened. Now load functions.
    if (!m_scenario.load(libpath))
    {
        m_error = "Failed loading the scenario: " + m_scenario.error();
        //m_message_bar.entry(m_error, sf::Color::Red);
        return false;
    }

    return init();
}

//------------------------------------------------------------------------------
bool Simulator::autoreload()
{
    // Auto reload the scenario file if it has changed.
    if (m_scenario.autoreload())
    {
        // Check if it all functions are not nullptr
        if (!m_scenario.valid())
        {
            m_error = "Failed loading the scenario: " + m_scenario.error();
            //m_message_bar.entry(m_error, sf::Color::Red);
            return false;
        }
        else
        {
            //m_message_bar.entry("Scenario changed: reloaded", sf::Color::Yellow);
            return init();
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool Simulator::init()
{
    // Missing call Simulator::load() or scenario has failed loaded.
    if (!m_scenario.valid())
    {
        //m_message_bar.entry("No scenario referred", sf::Color::Red);
        return false;
    }

    // Set simulation name on the GUI
    //m_message_bar.entry("Starting simulation '" + m_scenario.name() + "'", sf::Color::Green);

    // Create a new city from "scratch".
    m_city.reset();
    m_ego = &m_scenario.createCity(*this, m_city);

    // Make by default, the camera follows the ego car.
    follow(*m_ego);

    // Clear simulation time
    m_pause = false;
    m_elpased_time = sf::Time::Zero;
    m_clock.restart();

    // Start recoring simulation states.
    // FIXME do not hardcode the file path.
    // FIXME do not smash old simulation. Add id++ or time
    monitor.open("/tmp/monitor.csv", ';');
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
        //m_message_bar.entry("Pause the simulation", sf::Color::Yellow);
    }
    else
    {
        //m_message_bar.entry("Running the simulation", sf::Color::Yellow);
        m_clock.restart();
    }
}

//------------------------------------------------------------------------------
void Simulator::activate()
{
    LOGI("Simulator: activated");
    pause(false);
}

//------------------------------------------------------------------------------
void Simulator::deactivate()
{
    LOGI("Simulator: deactivated");
    pause(true);
}

//------------------------------------------------------------------------------
void Simulator::release()
{
    m_city.reset();
    m_scenario.unload();
    monitor.close();
    m_elpased_time = sf::Time::Zero;
}

//------------------------------------------------------------------------------
void Simulator::reacts(size_t key)
{
    if ((!m_scenario.valid()) || (m_pause))
        return ;

    m_scenario.reactTo(*this, key);
}

//------------------------------------------------------------------------------
bool Simulator::continuing() const
{
    return m_scenario.valid()
        && (m_scenario.haltWhen(*this) != Scenario::Status::Continue);
}

//------------------------------------------------------------------------------
void Simulator::collisions(Car& ego)
{
#if 0
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
#endif
}

//------------------------------------------------------------------------------
void Simulator::update(const Second dt)
{
    // Hot reload the scenario
    autoreload();

    // User has paused the simulation ?
    if (m_pause)
    {
        //m_message_bar.entry("The simulation is in pause", sf::Color::Yellow);
        return ;
    }

    // Update physics, ECU, sensors of all NPC vehicles ...
    for (auto& it: m_city.cars())
    {
        it->update(dt);
    }

#if 0
    // Update physics, ECU, sensors of the Ego vehicle
    m_ego->update(dt);
    collisions(*m_ego);

    // Update parkings
    for (auto& it: m_city.parkings())
    {
        it->update(dt);
    }
#endif

    // Make the camera follows the car
    if (m_follow != nullptr)
    {
        const sf::Vector2<Meter> p = m_follow->position();
        m_camera = sf::Vector2f(float(p.x.value()), float(p.y.value()));
    }

    // Record simulation states
    monitor.record(elapsedTime());
}