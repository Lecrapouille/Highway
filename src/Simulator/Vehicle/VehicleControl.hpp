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

#ifndef VEHICLE_CONTROL_HPP
#  define VEHICLE_CONTROL_HPP

#  include "Math/Units.hpp"

class VehicleControl
{
public:

    //----------------------------------------------------------------------
    //! \brief The desired values
    //----------------------------------------------------------------------
    struct References
    {
        MeterPerSecond speed = 0.0_mps;
        Radian steering = 0.0_rad;
    };

    //----------------------------------------------------------------------
    //! \brief The desired values
    //----------------------------------------------------------------------
    struct Inputs
    {
        float throttle = 0.0f; // [%]
        float brake = 0.0f; // [%]
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    struct Outputs
    {
        MeterPerSecond speed = 0.0_mps;
        Radian steering = 0.0_rad;
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_speed(MeterPerSecond const speed)
    {
        ref.speed = speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    MeterPerSecond get_ref_speed()
    {
        return ref.speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_steering(Radian const steering)
    {
        ref.steering = steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    Radian get_ref_steering()
    {
        return ref.steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    Radian get_steering()
    {
        return outputs.steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    MeterPerSecond get_speed()
    {
        return outputs.speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_acceleration(MeterPerSecondSquared const acc, Second const dt)
    {
        ref.speed += acc * dt;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_steering_rate(RadianPerSecond const steering_rate, Second const dt)
    {
        ref.steering += steering_rate * dt;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
// TODO init

    //----------------------------------------------------------------------
    //! \brief TODO
    //----------------------------------------------------------------------
    void update(Second const /*dt*/ /*, sensors*/)
    {
        //
        outputs.speed = ref.speed;
        outputs.steering = ref.steering;
    }

    References ref;
    Inputs inputs;
    Outputs outputs;
};

#endif
