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

#  include "Core/Common/DynamicLoader.hpp"
#  include <functional>

class Simulator;
class City;
class Car;

// ****************************************************************************
//! \brief Struct holding functions loaded from a shared library depicted in
//! Highway/Scenarios/API.hpp for defining new simulation scenarios for Highway.
//! This class is loaded by the class \c Simulator.
// ****************************************************************************
struct Scenario
{
    enum class Status { Failed = -1, Continue = 0, Succeeded = 1 };

    //! \brief C function returning the name of the simulation.
    std::function<const char* (void)> function_scenario_name = nullptr;
    //! \brief C function taking a City as input and allows to create the
    //! desired city (roads, parking, vehicles ...). This function returns the
    //! ego car.
    std::function<Car&(Simulator&, City&)> function_create_city = nullptr;
    //! \brief C function taking the Simulator as input and returns true when
    //! the simulation shall halt (simulation duration, collision, reach a given
    //! position ...).
    std::function<int(Simulator const&)> function_halt_when = nullptr;
    //! \brief C function Reacting to external events (referred by their ID for
    //! example keyboard keys).
    std::function<void(Simulator&, size_t const)> function_react_to = nullptr;

    //-------------------------------------------------------------------------
    bool lookup(DynamicLoader /*const*/& dynamic_loader) //const
    {
        function_scenario_name = dynamic_loader.lookup<const char* (void)>("scenario_name");
        function_create_city = dynamic_loader.lookup<Car& (Simulator&, City&)>("scenario_create_city");
        function_halt_when = dynamic_loader.lookup<int(Simulator const&)>("scenario_halt_when");
        function_react_to = dynamic_loader.lookup<void(Simulator&, size_t)>("scenario_react_to");
        return isValid();
    }

    //-------------------------------------------------------------------------
    bool isValid() const
    {
        return (function_scenario_name != nullptr) &&
            (function_create_city != nullptr) &&
            (function_halt_when != nullptr) &&
            (function_react_to != nullptr);
    }

    //-------------------------------------------------------------------------
    void reset()
    {
        function_scenario_name = nullptr;
        function_create_city = nullptr;
        function_halt_when = nullptr;
        function_react_to = nullptr;
    }
};