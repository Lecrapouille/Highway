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

#include "Core/Simulator/Vehicle/VehicleShape.hpp"
#include "Core/Math/Collide.hpp"
#include "Core/Math/Math.hpp"

//------------------------------------------------------------------------------
VehicleShape::VehicleShape(vehicle::BluePrint const& p_blueprint)
    : blueprint(p_blueprint)
{
    // Origin on the middle of the rear wheel axle
    m_obb.setSize(sf::Vector2f(float(blueprint.length.value()),
                               float(blueprint.width.value())));
    m_obb.setOrigin(sf::Vector2f(float(blueprint.back_overhang.value()),
                                 m_obb.getSize().y / 2.0f));
}

//------------------------------------------------------------------------------
void VehicleShape::update(sf::Vector2<Meter> const& position, Radian const heading)
{
    // Update body shape
    m_obb.setPosition(float(position.x.value()), float(position.y.value()));
    m_obb.setRotation(float(Degree(heading).value()));

    // Update wheel shape
    //size_t i(BLUEPRINT::Where::MAX);
    //while (i--)
    //{
    //    blueprint.wheels[i].position =
    //        position + math::heading(m_blueprint.wheels[i].offset, heading);
    //}

    // TODO scene graph pour faire tourner les autre formes.
}