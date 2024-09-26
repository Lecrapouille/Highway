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

#pragma once

#  include "Core/Scenario/DynamicLoader.hpp"

#  if defined(_WIN32)
#    define SHARED_LIB_EXTENSION "dll"
#  elif defined(__linux__)
#    define SHARED_LIB_EXTENSION "so"
#  elif defined(__APPLE__)
#    define SHARED_LIB_EXTENSION "dylib"
#  endif

class City;
class Car;
class Simulator;

// ****************************************************************************
//! \brief Class holding functions loaded from a shared library depicted in
//! Highway/Scenarios/API.hpp for defining new simulation scenarios for Highway.
//! This class is loaded by the class \c Simulator.
// ****************************************************************************
class Scenario
{
public:

    enum class Status { Failed = -1, Continue = 0, Succeeded = 1 };

    ~Scenario();

    //-------------------------------------------------------------------------
    //! \brief Load the the scenario file.
    //-------------------------------------------------------------------------
    bool load(fs::path const& libpath);
    void unload();
    bool valid() const;

    //-------------------------------------------------------------------------
    //! \brief If the load failed, get the reason.
    //-------------------------------------------------------------------------
    std::string const& error() const;

    //-------------------------------------------------------------------------
    //! \brief Auto reload the scenario file if it has changed.
    //-------------------------------------------------------------------------
    bool autoreload();

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    std::string name() const;

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Car& createCity(Simulator& simulator, City& city) const;

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void reactTo(Simulator& simulator, size_t event) const;

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Scenario::Status haltWhen(Simulator const& simulator) const;

private:

    bool loadSymbols();
    void unloadSymbols();

private:

    //! \brief C function returning the name of the simulation.
    std::function<const char* (void)> m_function_scenario_name = nullptr;
    //! \brief C function taking a City as input and allows to create the
    //! desired city (roads, parking, vehicles ...). This function returns the
    //! ego car.
    std::function<Car&(Simulator&, City&)> m_function_create_city = nullptr;
    //! \brief C function taking the Simulator as input and returns true when
    //! the simulation shall halt (simulation duration, collision, reach a given
    //! position ...).
    std::function<int(Simulator const&)> m_function_halt_when = nullptr;
    //! \brief C function Reacting to external events (referred by their ID for
    //! example keyboard keys).
    std::function<void(Simulator&, size_t const)> m_function_react_to = nullptr;
    
private:

    DynamicLoader m_dynamic_loader;
};