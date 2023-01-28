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

#ifndef API_SCENARIO_HPP
#  define API_SCENARIO_HPP

// ****************************************************************************
//! \file A scenario for the standalone application Highway is a C++ code
//! compiled as shared library and implementing ALL functions depicted in this
//! current header file. These functions will be loaded by the Simulator class.
// ****************************************************************************

#  include "City/City.hpp"
#  include "Simulator/Simulator.hpp"

#  ifdef __cplusplus
extern "C" {
#  endif

//-----------------------------------------------------------------------------
//! \brief C++ function returning the simulation name (used for title and
//! description). Do not allocate the string: a simple return "Simulation name"
//! is enough.
//-----------------------------------------------------------------------------
const char* simulation_name();

//-----------------------------------------------------------------------------
//! \brief C++ function taking the simulator as input (passed by the
//! application) and returning true when the simulation shall halt. Here some
//! ideas: when the ego car collides with any element of the city, by timeout
//! (the ego car did not reach its destination within a given time), or the ego
//! car is outside the world, the ego car has stalled ...
//!
//! \param[in] simulator: Highway's simulator class.
//!
//! \return CONTINUE_SIMULATION (false) if the simulation shall continues or
//! return ABORT_SIMULATION (true) to halt the simulation.
//-----------------------------------------------------------------------------
bool simulation_halt_when(Simulator const& simulator);

//-----------------------------------------------------------------------------
//! \brief Helper macro to be returned by \c halt_simulation_when to continue
//! the simulation.
//-----------------------------------------------------------------------------
#define CONTINUE_SIMULATION return false

//-----------------------------------------------------------------------------
//! \brief Helper macro to be returned by \c halt_simulation_when to abort
//! the simulation.
//-----------------------------------------------------------------------------
#define ABORT_SIMULATION return true

//-----------------------------------------------------------------------------
//! \brief Helper macro for \c halt_simulation_when to abort the simulation.
//! \param[in] condition: the condition to halt the simulation when true.
//! \param[in] txt: text explaining why the simulation has halted.
//-----------------------------------------------------------------------------
#define HALT_SIMULATION_WHEN(condition, txt) \
   if (condition) { \
      simulator.messagebox(txt, sf::Color::Yellow); \
      return true; \
   }

//-----------------------------------------------------------------------------
//! \brief C++ fonction taking a city as input-output (given by the application)
//! to allow your to creating your desired city (ie roads, parkings, cars,
//! pedestrians ...). You shall create your ego vehicle with city.addEgo(...)
//! and ie attach to it some sensors. This function shall return the ego car.
//!
//! \param[inout] The dummy city to construct (roads, vehicles, parkings, ego
//! car ...). The city shall holds one and only one ego vehicle.
//!
//! \return the reference to the ego vehicle.
//-----------------------------------------------------------------------------
Car& simulation_create_city(Simulator& simulator, City& city);

//-----------------------------------------------------------------------------
//! \brief C++ fonction allows the simulator to react to keyboard events.
//!
//! \param[inout] simulator class passed by the application.
//! \param[in] unique identifier of the event to react to.
//-----------------------------------------------------------------------------
void simulation_react_to(Simulator& simulator, size_t const key);

#  ifdef __cplusplus
}
#  endif

#endif /* API_SCENARIO_HPP */
