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
bool Simulator::load(fs::path const& libpath)
{
    LOGI("Simulator loads scenario '%s'", libpath);

    if (m_dynamic_loader.load(libpath, DynamicLoader::ResolveTime::Now,
        DynamicLoader::Visiblity::Local))
    {
        if (loadSymbols())
        {
            return init();
        }
        else
        {
            messagebar(mylogger::Severity::Error, "Scenario has dummy symbols ", libpath);
            return false;
        }
    }
    else
    {
        messagebar(mylogger::Severity::Error, "Scenario failed loading '", libpath,
            "'. Reason was '", error());
        m_scenario.reset();
        return false;
    }
}

//------------------------------------------------------------------------------
bool Simulator::load(Scenario const& scenario)
{
    LOGI("Simulator loads embedded scenario");

    m_scenario = scenario;
    if (!m_scenario.isValid())
    {
        messagebar(mylogger::Severity::Error, "Failed loading the embedded scenario");
        return false;
    }

    return init();
}

//------------------------------------------------------------------------------
bool Simulator::autoreload()
{
    // Auto reload the scenario file if it has changed.
    if (m_dynamic_loader.reload())
    {
        if (!loadSymbols())
        {
            messagebar(mylogger::Severity::Error, "Failed loading the scenario: ",
                m_dynamic_loader.error());
            return false;
        }
        else
        {
            messagebar(mylogger::Severity::Info, "Scenario changed: reloaded");
            return init();
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool Simulator::loadSymbols()
{
    try
    {
        return m_scenario.lookup(m_dynamic_loader);
    }
    catch (...)
    {
        m_scenario.reset();
        return false;
    }
}

//------------------------------------------------------------------------------
std::string Simulator::scenarioName() const
{
    if (m_scenario.function_scenario_name != nullptr)
        return m_scenario.function_scenario_name();
    return {};
}

//------------------------------------------------------------------------------
Car& Simulator::createCity(Simulator& simulator, City& city) const
{
    if (m_scenario.function_create_city == nullptr)
    {
        throw std::logic_error("createCity dummy symbol from scenario file");
    }
    return m_scenario.function_create_city(simulator, city);
}

//------------------------------------------------------------------------------
void Simulator::reactTo(Simulator& simulator, size_t event) const
{
    if (m_scenario.function_react_to != nullptr)
        m_scenario.function_react_to(simulator, event);
}

//------------------------------------------------------------------------------
Scenario::Status Simulator::haltWhen(Simulator const& simulator) const
{
    if (m_scenario.function_halt_when != nullptr)
        return static_cast<Scenario::Status>(m_scenario.function_halt_when(simulator));
    // messagebar("Failed loading scenario file", sf::Color::Red);
    return Scenario::Status::Failed;
}

//------------------------------------------------------------------------------
bool Simulator::init()
{
    // Missing call Simulator::load() or scenario has failed loaded.
    if (!m_scenario.isValid())
    {
        messagebar(mylogger::Severity::Error, "No scenario referred");
        return false;
    }

    // Set simulation name on the GUI
    messagebar(mylogger::Severity::None, "Starting simulation '", scenarioName(), "'");

    // Create a new city from "scratch".
    m_city.reset();
    m_ego = &createCity(*this, m_city);

    // Make by default, the camera follows the ego car.
    follow(*m_ego);

    // Clear simulation time
    m_pause = false;
    m_elapsed_time = sf::Time::Zero;
    m_clock.restart();

    // Start recording simulation states.
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
        m_elapsed_time += m_clock.getElapsedTime();
        messagebar(mylogger::Severity::Warning, "Pause the simulation");
    }
    else
    {
        messagebar(mylogger::Severity::Warning, "Running the simulation");
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
    m_dynamic_loader.close();
    monitor.close();
    m_elapsed_time = sf::Time::Zero;
}

//------------------------------------------------------------------------------
void Simulator::reacts(size_t key)
{
    if ((!m_scenario.isValid()) || (m_pause))
        return ;

    reactTo(*this, key);
}

//------------------------------------------------------------------------------
bool Simulator::continuing() const
{
    return /*m_scenario.valid() &&*/ (haltWhen(*this) == Scenario::Status::Continue);
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
        messagebar(mylogger::Severity::Info, "The simulation is in pause");
        return ;
    }

    // Update physics, ECU, sensors of all NPC vehicles ...
    for (auto& it: m_city.cars())
    {
        it->update(dt);
    }

    // Update physics, ECU, sensors of the Ego vehicle
    m_ego->update(dt);

#if 0
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
