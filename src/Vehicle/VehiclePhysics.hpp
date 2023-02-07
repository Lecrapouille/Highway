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

#ifndef VEHICLE_PHYSICS_HPP
#  define VEHICLE_PHYSICS_HPP

#  include "Math/Movable.hpp"
#  include "Vehicle/VehicleShape.hpp"
#  include "Vehicle/VehicleControl.hpp"

// *****************************************************************************
//! \brief Base class for computing the vhecile physic (kinematic, dynamics ...)
//! need to implement void Movable::update(float const dt)
// *****************************************************************************
template<class BLUEPRINT>
class VehiclePhysics : public Movable
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor: aggregation of the vehicle shape and its
    //! control.
    //--------------------------------------------------------------------------
    VehiclePhysics(VehicleShape<BLUEPRINT> const& shape, VehicleControl const& control)
        : m_shape(shape), m_control(control)
    {}

    //--------------------------------------------------------------------------
    //! \brief Needed beause of virtual methods.
    //--------------------------------------------------------------------------
    virtual ~VehiclePhysics() = default;

protected:

    //! \brief
    VehicleShape<BLUEPRINT> const& m_shape;
    //! \brief
    VehicleControl const& m_control;
};

#endif
