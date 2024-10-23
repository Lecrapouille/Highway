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

#include "Core/Simulator/Vehicle/BluePrint.hpp"
#include <cassert>

namespace vehicle {

//------------------------------------------------------------------------------
BluePrint::BluePrint(const Meter p_length, const Meter p_width,
                     const Meter p_wheelbase, const Meter p_back_overhang,
                     const Meter p_wheel_radius, const Meter p_wheel_thickness,
                     const Meter p_turning_diameter, const double p_steering_ratio)
    : length(p_length), width(p_width), track(p_width - p_wheel_thickness),
      wheelbase(p_wheelbase), back_overhang(p_back_overhang),
      front_overhang(p_length - p_wheelbase - p_back_overhang),
      steering_ratio(p_steering_ratio)
{
    max_steering_angle = units::math::asin(wheelbase / (0.5f * p_turning_diameter));
    assert(max_steering_angle > 0.0_deg);
    assert(max_steering_angle < 90.0_deg);

    // Wheels
    const Meter K = track / 2.0f;
    wheels.resize(BluePrint::Where::MAX);
    wheels[BluePrint::Where::FL].offset = sf::Vector2<Meter>(wheelbase, K);
    wheels[BluePrint::Where::FR].offset = sf::Vector2<Meter>(wheelbase, -K);
    wheels[BluePrint::Where::RL].offset = sf::Vector2<Meter>(0.0_m, K);
    wheels[BluePrint::Where::RR].offset = sf::Vector2<Meter>(0.0_m, -K);
    for (auto& wheel: wheels)
    {
        wheel.radius = p_wheel_radius;
        wheel.thickness = p_wheel_thickness;
    }

    // Turning indicators
    turning_indicators.resize(BluePrint::Where::MAX);
    turning_indicators[BluePrint::Where::FL].offset = sf::Vector2<Meter>(wheelbase + front_overhang, K);
    turning_indicators[BluePrint::Where::FR].offset = sf::Vector2<Meter>(wheelbase + front_overhang, -K);
    turning_indicators[BluePrint::Where::RL].offset = sf::Vector2<Meter>(-back_overhang, K);
    turning_indicators[BluePrint::Where::RR].offset = sf::Vector2<Meter>(-back_overhang, -K);
    for (auto& it: turning_indicators)
    {
        it.dimension = sf::Vector2{0.1_m, 0.1_m};
    }

    // Lights
    const Meter K1 = K - 0.1_m; // 0.1: Turning indicators size
    lights.resize(BluePrint::Where::MAX);
    lights[BluePrint::Where::FL].offset = sf::Vector2<Meter>(wheelbase + front_overhang, K1);
    lights[BluePrint::Where::FR].offset = sf::Vector2<Meter>(wheelbase + front_overhang, -K1);
    lights[BluePrint::Where::RL].offset = sf::Vector2<Meter>(-back_overhang, K1);
    lights[BluePrint::Where::RR].offset = sf::Vector2<Meter>(-back_overhang, -K1);
    for (auto& it: lights)
    {
        it.dimension = sf::Vector2{0.1_m, 0.1_m};
    }
}

} // namespace vehicle