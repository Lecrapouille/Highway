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

#ifndef TRICYCLE_DYNAMIC_HPP
#  define TRICYCLE_DYNAMIC_HPP

#include "Vehicle/VehiclePhysics.hpp"

// ****************************************************************************
//! \brief
// ****************************************************************************
class PowerTrain
{
public:

    //-------------------------------------------------------------------------
    virtual ~PowerTrain() = default;

    //-------------------------------------------------------------------------
    float update(float const dt, float const throttle,
                 /*float const brake,*/ float const load);

private:

    //-------------------------------------------------------------------------
    // Throttle to engine torque using a simplified quadratic model.
    //-------------------------------------------------------------------------
    virtual float throttle2Torque(float const throttle, float const speed);

private:

    float m_torque_engine = 0.0f;
    float m_speed_engine = 100.0f;
    float m_gear_ratio = 0.35f;
    float m_inertia = 10.0f;

public:

    const float effective_radius = 0.3f;
};

// *****************************************************************************
//! \brief Simple car dynamic using the tricycle dynamic equations.
//! The position (x, y) of the car is the middle of the rear axle.
// *****************************************************************************
class TricycleDynamic: public VehiclePhysics // FIXME https://github.com/Lecrapouille/Highway/issues/34
{
public:

    // https://github.com/quangnhat185/Self-driving_cars_toronto_coursera/blob/master/1.%20Introduciton%20to%20Self-driving%20Cars/Longitudinal_Vehicle_Model.ipynb
    virtual void update(CarControl const& control, float const dt) override;

private:

    PowerTrain m_powertrain;
    Monitoring m_monitor;
};

#endif
