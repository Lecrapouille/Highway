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

#  include "Core/Simulator/Vehicle/PhysicModel.hpp"
//#  include "Vehicle/VehicleBluePrint.hpp"

#if 0
struct CarBluePrint
{
    Meter wheelbase = ;
};

Meter& wheelbase(CarBluePrint& bp) { return bp.wheelbase; }

#endif

namespace vehicle {

// *****************************************************************************
//! \brief Simple car kinematic using the tricycle kinematic equations.
//! The position (x, y) of the car is the middle of the rear axle.
//! ../../doc/pics/TricycleVehicle.png
//! ../../doc/pics/TricycleKinematicEq.png
// *****************************************************************************
class TricycleKinematic: public PhysicModel
{
public:

    //--------------------------------------------------------------------------
    TricycleKinematic(VehicleShape const& shape)
        : PhysicModel(shape),
          m_wheelbase(shape.blueprint.wheelbase)
    {}

    //--------------------------------------------------------------------------
    TricycleKinematic(VehicleShape const& shape,
                      MeterPerSecondSquared const acceleration, // Comment supprimmer ?
                      MeterPerSecond const speed,
                      sf::Vector2<Meter> const position,
                      Radian const heading)
        : PhysicModel(shape),
          m_wheelbase(shape.blueprint.wheelbase)
    {
        init(acceleration, speed, position, heading);
    }

    //--------------------------------------------------------------------------
    //! \brief Set initial state values needed by physical equations.
    //! \param[in] position: the (x, y) world coordinated of the car (can be the
    //!   middle of the rear axle).
    //! \param[in] speed: initial longitudinal speed [meter / second].
    //! \param[in] position: initial world position of the car (its center of
    //!   the rear axle) [meter].
    //! \param[in] heading: the initial yaw angle of the vehicle [radian].
    //--------------------------------------------------------------------------
    virtual void init(MeterPerSecondSquared const /*acceleration*/, MeterPerSecond const speed,
                     sf::Vector2<Meter> const position, Radian const heading) override
    {
        // FIXME how to remove acceleration explicitely ?
        Movable::init(0.0_mps_sq, speed, position, heading);
    }

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
    virtual void update(Second const dt) override
    {
        constexpr Radian u = 1_rad; // For converting radian to no unit.

        //FIXME m_speed = m_control.outputs.speed;
        //FIXME m_heading += dt * m_speed * u * units::math::tan(m_control.outputs.steering) / m_wheelbase;
        m_position.x += dt * m_speed * units::math::cos(m_heading);
        m_position.y += dt * m_speed * units::math::sin(m_heading);
    }

protected:

   Meter const& m_wheelbase;
};

} // namespace vehicle