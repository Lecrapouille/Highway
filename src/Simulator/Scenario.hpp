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

#ifndef SCENARIO_HPP
#  define SCENARIO_HPP

#  if defined(_WIN32)
#    define SHARED_LIB_EXTENSION "dll"
#  elif defined(__linux__)
#    define SHARED_LIB_EXTENSION "so"
#  elif defined(__APPLE__)
#    define SHARED_LIB_EXTENSION "dylib"
#  endif

class City;
class Simulator;

// ****************************************************************************
//! \brief Class holding functions loaded from a shared library depicted in
//! Highway/Scenarios/API.hpp for defining new simulation scenarios for Highway.
//! This class is loaded by the class \c Simulator.
// ****************************************************************************
class Scenario
{
public:

    //-------------------------------------------------------------------------
    //! \brief Return true if all functions have been set. None of them shall
    //! be nullptr.
    //-------------------------------------------------------------------------
    inline operator bool() const
    {
        return (name != nullptr) && (create != nullptr) &&
               (halt != nullptr) && (react != nullptr);
    }

    //-------------------------------------------------------------------------
    //! \brief Helper to reset all functions to nullptr invalidating this class
    //! instance.
    //-------------------------------------------------------------------------
    void clear()
    {
        name = nullptr; create = nullptr; halt = nullptr; react = nullptr;
    }

public:

    //! \brief C fonction returning the name of the simulation.
    std::function<const char* (void)> name = nullptr;
    //! \brief C fonction taking a City as input and allows to create the
    //! desired city (roads, parking, vehicles ...). This function returns the
    //! ego car.
    std::function<Car&(City&)> create = nullptr;
    //! \brief C function taking the Simulator as input and returns true when
    //! the simulation shall halt (simulation duration, collision, reach a given
    //! position ...).
    std::function<bool(Simulator const&)> halt = nullptr;
    //! \brief C function Reacting to external events (refered by their ID for
    //! example keyboard keys).
    std::function<void(Simulator& simulator, size_t const key)> react = nullptr;
};

#endif
