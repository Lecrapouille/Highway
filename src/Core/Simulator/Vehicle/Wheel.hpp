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

#  include "Core/Math/Units.hpp"

// *****************************************************************************
//! \brief Vehicle's wheel. Note that the shape is defined by WheelBluePrint in
//! VehicleBluePrint.hpp
// *****************************************************************************
struct Wheel
{
    //! \brief current position and altitude inside the world coordinate.
    sf::Vector2<Meter> position; // TODO sf::Vector3<Meter>
    //! \brief Yaw angle [rad].
    Radian steering;
    //! \brief Angular speed [meter / second]
    MeterPerSecond speed;
};