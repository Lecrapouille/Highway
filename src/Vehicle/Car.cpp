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

#  include "MyLogger/Logger.hpp"
#  include "Simulation/BluePrints.hpp"
#  include "Vehicle/Car.hpp"
#  include "Vehicle/VehiclePhysicalModels/TricycleKinematic.hpp"

//------------------------------------------------------------------------------
Car::Car(const char* name_, sf::Color const& color_)
    : Vehicle<CarBluePrint>(BluePrints::get<CarBluePrint>(name_), name_, color_)
{
    m_physics = std::make_unique<TricycleKinematic>(*m_shape, *m_control);
}

//------------------------------------------------------------------------------
void Car::update_wheels(MeterPerSecond const speed, Radian const steering)
{
    m_wheels[CarBluePrint::Where::FL].speed = speed;
    m_wheels[CarBluePrint::Where::FR].speed = speed;
    m_wheels[CarBluePrint::Where::RL].speed = speed;
    m_wheels[CarBluePrint::Where::RR].speed = speed;

    m_wheels[CarBluePrint::Where::FL].steering = steering;
    m_wheels[CarBluePrint::Where::FR].steering = steering;
    m_wheels[CarBluePrint::Where::RL].steering = 0.0_rad;
    m_wheels[CarBluePrint::Where::RR].steering = 0.0_rad;
}
