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

#ifndef HIGHWAY_SCENARIO_HPP
#  define HIGHWAY_SCENARIO_HPP

// ****************************************************************************
//! \file A scenario for the standalone application Highway is a C++ code
//! compiled as shared library and implementing ALL functions depicted in this
//! current header file. These functions will be loaded by the Simulator class.
// ****************************************************************************

#  include "Core/Simulator/Simulator.hpp"

#  ifdef __cplusplus
extern "C" {
#  endif

//-----------------------------------------------------------------------------
//! \brief C++ function returning the scenario name (used for title and
//! description). Do not allocate the string: a simple return "Simulation name"
//! is enough.
//-----------------------------------------------------------------------------
const char* scenario_name();

//-----------------------------------------------------------------------------
//! \brief C++ function takes the simulator as input (passed by the application)
//! and returns enum indicating if the scenario shall continue, halt with failure
//! or halt with success.
//!
//! Here some ideas of halting because of failures: the ego car collides with an
//! element of the city (road border or cars), by timeout of the simulation (the
//! ego car did not reach its destination within a given time), the ego car is
//! outside the world, the ego car has stalled ...
//!
//! Here some ideas of halting because of success: the ego car reach its goal.
//!
//! \todo To be defined add higher level functions taking into account the result
//! and redoing the scenario with different parameters (genetic algorithms ...)
//!
//! \param[in] simulator: Highway's simulator class.
//!
//! \return 0 to continue the simulation. Negative for halting with failure.
//! Positive value for halting with success.
//-----------------------------------------------------------------------------
int scenario_halt_when(Simulator const& simulator);

//-----------------------------------------------------------------------------
//! \brief Helper macro to be returned by \c scenario_status to continue
//! the scenario.
//-----------------------------------------------------------------------------
#define SCENARIO_CONTINUES return 0 // Scenario::Status::Continue

//-----------------------------------------------------------------------------
//! \brief Helper macro for \c scenario_status to halt with failure the scenario.
//! \param[in] condition: the condition to halt the scenario when true.
//! \param[in] txt: text explaining why the scenario has halted.
//-----------------------------------------------------------------------------
#define SCENARIO_FAILS_WHEN(condition, txt)                                   \
   if (condition) {                                                           \
      simulator.messagebar(txt, sf::Color::Red);                              \
      return -1; /*Scenario::Status::Failed;*/                                \
   }

//-----------------------------------------------------------------------------
//! \brief Helper macro for \c scenario_status to halt with success the scenario.
//! \param[in] condition: the condition to halt the scenario when true.
//! \param[in] txt: text explaining why the scenario has halted.
//-----------------------------------------------------------------------------
#define SCENARIO_SUCCEEDS_WHEN(condition, txt)                                \
   if (condition) {                                                           \
      simulator.messagebar(txt, sf::Color::Green);                            \
      return 1; /*Scenario::Status::Succeeded;*/                              \
   }

//-----------------------------------------------------------------------------
//! \brief Helper macro for \c scenario_status to abort the scenario.
//-----------------------------------------------------------------------------
#define SCENARIO_ABORTED                                                      \
   SCENARIO_FAILS_WHEN(false,"Scenario aborted by user decision")

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
Car& scenario_create_city(Simulator& simulator, City& city);

//-----------------------------------------------------------------------------
//! \brief C++ fonction allows the simulator to react to keyboard events.
//!
//! \param[inout] simulator class passed by the application.
//! \param[in] unique identifier of the event to react to.
//-----------------------------------------------------------------------------
void scenario_react_to(Simulator& simulator, size_t const key);

#  ifdef __cplusplus
}
#  endif

#endif /* HIGHWAY_SCENARIO_HPP */
