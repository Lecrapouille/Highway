//=============================================================================
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
//=============================================================================

#ifndef DEMO_HPP
#  define DEMO_HPP

#  include "Simulator/Simulator.hpp"

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
//! \brief "Hello simulation" demo: set the scenario functions mandatory to
//! create the simulation.
//-----------------------------------------------------------------------------
Scenario simple_simulation_demo();

#endif