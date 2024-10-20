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
#  include "Core/Math/Math.hpp"

// ****************************************************************************
//! \brief Class representing the steering wheel.
// ****************************************************************************
class SteeringWheel
{
public:
    //-------------------------------------------------------------------------
    //! \brief 
    //-------------------------------------------------------------------------
    explicit SteeringWheel(vehicle::BluePrint const& bp)
        : m_blueprint(bp)
    {
    }

    //-------------------------------------------------------------------------
    //! \brief Turn the steering wheel with realistic limits.
    //-------------------------------------------------------------------------
    Radian turn(Radian const delta_angle)
    {
        const Radian max = m_blueprint.max_steering_angle * m_blueprint.steering_ratio;

        m_steering_angle += delta_angle;
        if (m_steering_angle > max)
            m_steering_angle = max;

        if (m_steering_angle < -max)
            m_steering_angle = -max;

        return m_steering_angle;
    }

    //-------------------------------------------------------------------------
    //! \brief Get the steering angle.
    //-------------------------------------------------------------------------
    inline Radian getSteeringAngle() const
    {
        return m_steering_angle;
    }

    //-------------------------------------------------------------------------
    //! \brief Convert steering angle to wheel angle using steering ratio.
    //-------------------------------------------------------------------------
    inline Radian getWheelAngle() const
    {
        return m_steering_angle / m_blueprint.steering_ratio;
    }

private:
    //! \brief Dimension of the vehicle
    vehicle::BluePrint const& m_blueprint;
    //! \brief Current steering angle
    Radian m_steering_angle = 0.0_rad;
};