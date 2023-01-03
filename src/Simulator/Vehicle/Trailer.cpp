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

#include "Simulator/Vehicle/Trailer.hpp"
// FIXME https://github.com/Lecrapouille/Highway/issues/16
// #include "Vehicle/VehiclePhysicalModels/TrailerKinematic.hpp"

//------------------------------------------------------------------------------
Trailer::Trailer(const char* name_, sf::Color const& color_)
    : Vehicle<TrailerBluePrint>(BluePrints::get<TrailerBluePrint>(name_), name_, color_)
{
    std::cout << "Trailer " << name_ << std::endl;
    //m_physics = std::make_unique<TrailerKinematic>(*m_shape, *m_control);
}

//------------------------------------------------------------------------------
void Trailer::update_wheels(MeterPerSecond const speed, Radian const steering)
{
    m_wheels[TrailerBluePrint::WheelName::RL].speed = speed;
    m_wheels[TrailerBluePrint::WheelName::RR].speed = speed;

    m_wheels[TrailerBluePrint::WheelName::RL].steering = 0.0_rad;
    m_wheels[TrailerBluePrint::WheelName::RR].steering = 0.0_rad;
}
