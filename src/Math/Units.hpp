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

#ifndef SI_UNITS_HPP
#  define SI_UNITS_HPP

#  pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wfloat-equal"
#      include "units.h"
#   pragma GCC diagnostic pop

using namespace units::literals;

using Meter = units::length::meter_t;
using Radian = units::angle::radian_t;
using Degree = units::angle::degree_t;
using RadianPerSecond = units::angular_velocity::radians_per_second_t;
using MeterPerSecondSquared = units::acceleration::meters_per_second_squared_t;
using MeterPerSecond = units::velocity::meters_per_second_t;
using Second = units::time::second_t;

// TODO https://github.com/Lecrapouille/Highway/issues/13
// using units::math::cos() vs std::cos()

#endif
