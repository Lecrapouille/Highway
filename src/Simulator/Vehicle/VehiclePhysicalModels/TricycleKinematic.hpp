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

#ifndef TRICYCLEKINEMATIC_HPP
#  define TRICYCLEKINEMATIC_HPP

#  include "Vehicle/VehiclePhysics.hpp"
#  include "Vehicle/VehicleBluePrint.hpp"

// *****************************************************************************
//! \brief Simple car kinematic using the tricycle kinematic equations.
//! The position (x, y) of the car is the middle of the rear axle.
//! ../../doc/pics/TricycleVehicle.png
//! ../../doc/pics/TricycleKinematicEq.png
// *****************************************************************************
class TricycleKinematic: public VehiclePhysics<CarBluePrint>
{
public:

    //--------------------------------------------------------------------------
    TricycleKinematic(VehicleShape<CarBluePrint> const& shape, VehicleControl const& control)
        : VehiclePhysics<CarBluePrint>(shape, control)
    {}

    //--------------------------------------------------------------------------
    //! \brief Update discrete time equations from continuous time equations
    //! described in these pictures:
    //! ../../doc/pics/TricycleVehicle.png
    //! ../../doc/pics/TricycleKinematicEq.png
    // in where:
    //  - L is the vehicle wheelbase [meter]
    //  - v is the vehicle longitudinal speed [meter / second]
    //  - theta is the car heading (yaw) [radian]
    //  - delta is the steering angle [radian]
    //--------------------------------------------------------------------------
    virtual void update(float const dt) override;
};

#endif
