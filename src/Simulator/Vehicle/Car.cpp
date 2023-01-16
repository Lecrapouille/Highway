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
#  include "Simulator/BluePrints.hpp"
#  include "Simulator/Vehicle/Car.hpp"
#  include "Simulator/Vehicle/VehiclePhysicalModels/TricycleKinematic.hpp"

//------------------------------------------------------------------------------
Car::Car(const char* name_, sf::Color const& color_)
    : Vehicle<CarBluePrint>(BluePrints::get<CarBluePrint>(name_), name_, color_)
{
    LOGI("Car %s", name_);
    m_physics = std::make_unique<TricycleKinematic>(*m_shape, *m_control);
}

//------------------------------------------------------------------------------
void Car::update_wheels(MeterPerSecond const speed, Radian const steering)
{
    m_wheels[CarBluePrint::WheelName::FL].speed = speed;
    m_wheels[CarBluePrint::WheelName::FR].speed = speed;
    m_wheels[CarBluePrint::WheelName::RL].speed = speed;
    m_wheels[CarBluePrint::WheelName::RR].speed = speed;

    m_wheels[CarBluePrint::WheelName::FL].steering = steering;
    m_wheels[CarBluePrint::WheelName::FR].steering = steering;
    m_wheels[CarBluePrint::WheelName::RL].steering = 0.0_rad;
    m_wheels[CarBluePrint::WheelName::RR].steering = 0.0_rad;
}
