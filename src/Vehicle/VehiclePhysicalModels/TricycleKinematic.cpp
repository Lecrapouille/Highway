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

#include "Vehicle/VehiclePhysicalModels/TricycleKinematic.hpp"

void TricycleKinematic::update(Second const dt)
{
    const Radian u = 1_rad;
    const Meter wb = m_shape.blueprint().wheelbase;
    m_speed = m_control.outputs.speed;
    m_heading += dt * m_speed * u * units::math::tan(m_control.outputs.steering) / wb;
    m_position.x += dt * m_speed * units::math::cos(m_heading);
    m_position.y += dt * m_speed * units::math::sin(m_heading);
}
