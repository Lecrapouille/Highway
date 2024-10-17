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

#pragma once

//--------------------------------------------------------------------------
//! \file Add System International Units to the project. Currently the library
//! Units is used https://github.com/nholthaus/units (placed in external/) In
//! this file we alias used SI units to allow using other SI lib.
//!
//! \fixme The whole code of this project is full of units::math::cos() instead
//! of std::cos(). How to wrap this to use other SI lib ? See issue
//! https://github.com/Lecrapouille/Highway/issues/13
//--------------------------------------------------------------------------

#  include <SFML/System/Vector2.hpp>
#  include <SFML/System/Vector3.hpp>

#  pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wfloat-equal"
#    pragma GCC diagnostic ignored "-Wsign-conversion"
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#      include "external/units/units.hpp"
#   pragma GCC diagnostic pop

using namespace units::literals;

using Meter = units::length::meter_t;
using SquareMeter = units::area::square_meter_t;
using Radian = units::angle::radian_t;
using Degree = units::angle::degree_t;
using RadianPerSecond = units::angular_velocity::radians_per_second_t;
using MeterPerSecondSquared = units::acceleration::meters_per_second_squared_t;
using MeterPerSecond = units::velocity::meters_per_second_t;
using Second = units::time::second_t;
using Newton = units::force::newton_t;
using NewtonMeter = units::torque::newton_meter_t;
using Watt = units::power::watt_t;
using Percent = units::concentration::percent_t;

using Rpm = double;

constexpr MeterPerSecondSquared GRAVITY = 9.80665_mps_sq;

namespace math {

//------------------------------------------------------------------------------
template<typename T> 
constexpr T nan()
{
   static_assert(std::numeric_limits<double>::has_quiet_NaN, "NaN not supported");
   return std::numeric_limits<T>::quiet_NaN();
}

}