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

#  include "Core/Simulator/Vehicle/BluePrint.hpp"
#  include <SFML/Graphics/RectangleShape.hpp>

#  include <vector>
#  include <cassert>
#  include <memory>

// FIXME better to use scene graph for turning wheels and sensors (while easy to manage manually)

// *****************************************************************************
//! \brief A vehicle shape is an oriented bounding box of the body (rectangle knowing its
//! position and orientation (here in a 2D world: the heading aka yaw). The shape
//! is used for the rendering process and the collision detection.
//! \tparam BLUEPRINT struct holding dimension of the shape (ie CarBluePrint,
//! TrailerBluePrint ... see VehicleBlueprint.hpp).
// *****************************************************************************
class VehicleShape
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor with the vehicle dimension (ie CarBluePrint,
    //! TrailerBluePrint ... see Vehicle_blueprint.hpp).
    //--------------------------------------------------------------------------
    explicit VehicleShape(vehicle::BluePrint const& p_blueprint);

    //--------------------------------------------------------------------------
    //! \brief Refresh the position and heading of shape and child shapes (ie
    //! sensor shapes).
    //--------------------------------------------------------------------------
    void update(sf::Vector2<Meter> const& position, Radian const heading);

    //--------------------------------------------------------------------------
    //! \brief const getter: return the oriented bounding box (OBB) of the body.
    //--------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const { return m_obb; }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates [meter].
    //--------------------------------------------------------------------------
    inline sf::Vector2<Meter> position() const
    {
        auto const& p = m_obb.getPosition();
        return sf::Vector2<Meter>(Meter(p.x), Meter(p.y));
    }

public:

    //! \brief Dimension of the vehicle.
    vehicle::BluePrint const& blueprint;

private:

    //! \brief Oriented bounding box for attitude and collision
    sf::RectangleShape m_obb;

    // FIXME other shapes: sensors ...
};