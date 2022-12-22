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

#ifndef ECU_HPP
#  define ECU_HPP

#  include "Math/Units.hpp"
#  include "Common/Components.hpp"

// ****************************************************************************
//! \brief
// ****************************************************************************
class ECU: public Component
{
public:

   COMPONENT_CLASSTYPE(ECU, Component);

public:

   virtual void update(Second const dt) = 0;
};

// ****************************************************************************
// Forward declaration of ECUs
// ****************************************************************************

class AutoParkECU;

#endif
