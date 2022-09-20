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

#include "Simulator/Vehicle/VehicleBluePrint.hpp"
#include "Math/Math.hpp"
#include <cassert>

constexpr float WHEEL_WIDTH = 0.1f;

//------------------------------------------------------------------------------
CarBluePrint::CarBluePrint(const float l, const float w, const float wb,
                           const float bo, const float wr, const float td)
    : length(l), width(w), track(width - WHEEL_WIDTH), wheelbase(wb),
      back_overhang(bo), front_overhang(length - wheelbase - back_overhang)
{
    max_steering_angle = asinf(wheelbase / (0.5f * td));
    assert(max_steering_angle > 0.0f);
    assert(max_steering_angle < DEG2RAD(90.0f));

    const float K = track / 2.0f;
    wheels[WheelName::FL].offset = sf::Vector2f(wheelbase, K);
    wheels[WheelName::FR].offset = sf::Vector2f(wheelbase, -K);
    wheels[WheelName::RL].offset = sf::Vector2f(0.0f, K);
    wheels[WheelName::RR].offset = sf::Vector2f(0.0f, -K);

    size_t i = WheelName::MAX;
    while (i--)
    {
        wheels[i].position = sf::Vector2f(NAN, NAN);
        wheels[i].radius = wr;
        wheels[i].width = WHEEL_WIDTH;
    }
}

//------------------------------------------------------------------------------
TrailerBluePrint::TrailerBluePrint(const float l, const float w, const float d,
                                   const float bo, const float wr)
    : length(l), width(w), track(width - WHEEL_WIDTH), wheelbase(d),
      back_overhang(bo)
{
    const float K = track / 2.0f;
    wheels[WheelName::RL].offset = sf::Vector2f(0.0f, -K);
    wheels[WheelName::RR].offset = sf::Vector2f(0.0f, K);

    size_t i = WheelName::MAX;
    while (i--)
    {
        wheels[i].position = sf::Vector2f(NAN, NAN);
        wheels[i].radius = wr;
        wheels[i].width = WHEEL_WIDTH;
    }
}
