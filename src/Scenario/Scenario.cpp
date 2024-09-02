//=============================================================================
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
//=============================================================================

#include "Scenario/Scenario.hpp"
#include "MyLogger/Logger.hpp"

//------------------------------------------------------------------------------
Scenario::~Scenario()
{
    unload();
}

//------------------------------------------------------------------------------
bool Scenario::load(fs::path const& libpath)
{
    if (m_dynamic_loader.load(libpath))
    {
        if (loadSymbols())
        {
            LOGI("Scenario loaded '%s'", libpath.c_str());
            return true;
        }
        else
        {
            LOGE("Scenario has dummy symbols '%s'", libpath.c_str());
            return true;
        }
    }
    else
    {
        LOGE("Scenario failed loading '%s'. Reason was '%s'", libpath.c_str(),
            error().c_str());
        unloadSymbols();
        return false;
    }
}

//------------------------------------------------------------------------------
bool Scenario::valid() const
{
    return (m_function_scenario_name != nullptr) &&
        (m_function_create_city != nullptr) &&
        (m_function_halt_when != nullptr) &&
        (m_function_react_to != nullptr);
}

//------------------------------------------------------------------------------
void Scenario::unload()
{
    unloadSymbols();
    m_dynamic_loader.close();
}

//------------------------------------------------------------------------------
bool Scenario::autoreload()
{
    if (m_dynamic_loader.reloadIfChanged())
    {
        loadSymbols();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
bool Scenario::loadSymbols()
{
    try
    {
        m_function_scenario_name = m_dynamic_loader.prototype<const char* (void)>("scenario_name");
        m_function_create_city = m_dynamic_loader.prototype<Car& (Simulator&, City&)>("scenario_create_city");
        m_function_halt_when = m_dynamic_loader.prototype<int(Simulator const&)>("scenario_halt_when");
        m_function_react_to = m_dynamic_loader.prototype<void(Simulator&, size_t)>("scenario_react_to");
        return valid();
    }
    catch (...)
    {
        unloadSymbols();
        return false;
    }
}

//------------------------------------------------------------------------------
void Scenario::unloadSymbols()
{
    m_function_scenario_name = nullptr;
    m_function_create_city = nullptr;
    m_function_halt_when = nullptr;
    m_function_react_to = nullptr;
}

//------------------------------------------------------------------------------
std::string const& Scenario::error() const
{
    return m_dynamic_loader.error();
}

//------------------------------------------------------------------------------
std::string Scenario::name() const
{
    if (m_function_scenario_name != nullptr)
        return m_function_scenario_name();
    return {};
}

//------------------------------------------------------------------------------
Car& Scenario::createCity(Simulator& simulator, City& city) const
{
    //static Car car;

    //if (m_function_create_city != nullptr)
        return m_function_create_city(simulator, city);
    //return car;
}

//------------------------------------------------------------------------------
void Scenario::reactTo(Simulator& simulator, size_t event) const
{
    if (m_function_react_to != nullptr)
        m_function_react_to(simulator, event);
}

//------------------------------------------------------------------------------
Scenario::Status Scenario::haltWhen(Simulator const& simulator) const
{
    if (m_function_halt_when != nullptr)
        return static_cast<Scenario::Status>(m_function_halt_when(simulator));
    //simulator.messagebox("Failed loading scenario file", sf::Color::Red);
    return Scenario::Status::Failed;
}