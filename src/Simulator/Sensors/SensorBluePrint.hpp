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

#ifndef SENSOR_BLUEPRINT_HPP
#  define SENSOR_BLUEPRINT_HPP

#  include <SFML/Graphics/RectangleShape.hpp>
#  include "Math/Math.hpp"

struct SensorBluePrint
{
   // FIXME: why needed ?
   SensorBluePrint() {}
   SensorBluePrint(sf::Vector2f off, float ori)
     : offset(off), orientation(DEG2RAD(ori))
   {}

   //! \brief Relative position from the car shape position (middle rear axle)
   sf::Vector2f offset;
   //! \brief Relative relative orientation
   float orientation;
};

#endif
