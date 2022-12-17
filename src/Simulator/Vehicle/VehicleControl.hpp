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

class VehicleControl
{
public:

    //----------------------------------------------------------------------
    //! \brief The desired values
    //----------------------------------------------------------------------
    struct References
    {
        float speed = 0.0f;
        float steering = 0.0f;
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
        float speed = 0.0f;
        float steering = 0.0f;
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_speed(float const speed)
    {
        ref.speed = speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    float get_ref_speed()
    {
        return ref.speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_steering(float const steering)
    {
        ref.steering = steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    float get_ref_steering()
    {
        return ref.steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    float get_steering()
    {
        return outputs.steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    float get_speed()
    {
        return outputs.speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_acceleration(float const acc, float const dt)
    {
        ref.speed += acc * dt;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_steering_rate(float const steering, float const dt)
    {
        ref.steering += steering * dt;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
// TODO init

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void update(float const /*dt*/ /*, sensors*/)
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
